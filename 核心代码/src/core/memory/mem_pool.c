/**
 * 内存池实现 - 玄隐定制内存管理器
 */

#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <intrin.h>

// 内存块头部
typedef struct MemBlockHeader {
    size_t size;                    // 块大小
    MemPoolType pool_type;          // 所属内存池
    uint64_t magic;                 // 魔数校验
    struct MemBlockHeader* next;    // 下一个块(调试用)
    const char* file;               // 分配文件
    int line;                       // 分配行号
    const char* func;               // 分配函数
} MemBlockHeader;

// 内存池结构
typedef struct {
    void* start;                    // 池起始地址
    size_t total_size;              // 总大小
    size_t used_size;               // 已使用大小
    MemPoolConfig config;           // 配置
    MemPoolStats stats;             // 统计
    CRITICAL_SECTION lock;          // 线程锁(Windows)
    MemBlockHeader* first_block;    // 第一个内存块(调试)
} MemoryPool;

// 全局内存池数组
static MemoryPool g_mem_pools[MEM_POOL_COUNT];

// 魔数定义
#define MEM_MAGIC_NUMBER 0x594E4155U  // "YUAN"的十六进制

// 调试模式
#ifdef _DEBUG
#define MEM_DEBUG 1
#else
#define MEM_DEBUG 0
#endif

// 对齐大小
#define MEM_ALIGNMENT 16
#define ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT - 1))

// ========== 内部函数 ==========

static void mem_pool_init(MemoryPool* pool, MemPoolType type) {
    const MemPoolConfig default_configs[MEM_POOL_COUNT] = {
        { 64 * 1024 * 1024, 256 * 1024 * 1024, 1, 1 }, // GLOBAL
        { 32 * 1024 * 1024, 128 * 1024 * 1024, 1, 0 }, // RENDER
        { 128 * 1024 * 1024, 512 * 1024 * 1024, 1, 0 }, // WORLD
        { 8 * 1024 * 1024, 32 * 1024 * 1024, 0, 0 },   // AUDIO
        { 4 * 1024 * 1024, 16 * 1024 * 1024, 0, 0 }    // TEMP
    };

    memset(pool, 0, sizeof(MemoryPool));
    pool->config = default_configs[type];
    InitializeCriticalSection(&pool->lock);
    
    // 分配内存
    pool->start = VirtualAlloc(NULL, pool->config.initial_size,
                              MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (pool->start) {
        pool->total_size = pool->config.initial_size;
    }
}

static void mem_pool_destroy(MemoryPool* pool) {
    if (!pool) return;
    
    DeleteCriticalSection(&pool->lock);
    
    if (pool->start) {
        VirtualFree(pool->start, 0, MEM_RELEASE);
    }
    
    memset(pool, 0, sizeof(MemoryPool));
}

static void* mem_pool_alloc(MemoryPool* pool, size_t size, 
                           const char* file, int line, const char* func) {
    if (!pool || size == 0) return NULL;
    
    EnterCriticalSection(&pool->lock);
    
    // 对齐大小
    size_t aligned_size = ALIGN_SIZE(size);
    size_t total_size = aligned_size + sizeof(MemBlockHeader);
    
    // 检查空间
    if (pool->used_size + total_size > pool->total_size) {
        // 尝试扩容
        size_t new_size = pool->total_size * 2;
        if (new_size > pool->config.max_size) {
            LeaveCriticalSection(&pool->lock);
            return NULL;
        }
        
        void* new_mem = VirtualAlloc(NULL, new_size,
                                    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!new_mem) {
            LeaveCriticalSection(&pool->lock);
            return NULL;
        }
        
        // 复制数据
        memcpy(new_mem, pool->start, pool->used_size);
        VirtualFree(pool->start, 0, MEM_RELEASE);
        
        pool->start = new_mem;
        pool->total_size = new_size;
    }
    
    // 分配内存块
    uint8_t* mem = (uint8_t*)pool->start + pool->used_size;
    MemBlockHeader* header = (MemBlockHeader*)mem;
    
    header->size = size;
    header->pool_type = (MemPoolType)(pool - g_mem_pools);
    header->magic = MEM_MAGIC_NUMBER;
    
#if MEM_DEBUG
    header->file = file;
    header->line = line;
    header->func = func;
    header->next = pool->first_block;
    pool->first_block = header;
#endif
    
    void* user_mem = mem + sizeof(MemBlockHeader);
    
    // 零初始化
    if (pool->config.enable_zero_init) {
        memset(user_mem, 0, size);
    }
    
    pool->used_size += total_size;
    
    // 更新统计
    pool->stats.total_allocated += size;
    pool->stats.current_used += size;
    pool->stats.allocation_count++;
    
    if (pool->stats.current_used > pool->stats.peak_used) {
        pool->stats.peak_used = pool->stats.current_used;
    }
    
    LeaveCriticalSection(&pool->lock);
    return user_mem;
}

static void mem_pool_free(MemoryPool* pool, void* ptr) {
    if (!pool || !ptr) return;
    
    EnterCriticalSection(&pool->lock);
    
    uint8_t* mem = (uint8_t*)ptr - sizeof(MemBlockHeader);
    MemBlockHeader* header = (MemBlockHeader*)mem;
    
    // 验证魔数
    if (header->magic != MEM_MAGIC_NUMBER) {
        // 可能不是我们分配的内存
        LeaveCriticalSection(&pool->lock);
        return;
    }
    
    // 更新统计
    pool->stats.current_used -= header->size;
    pool->stats.free_count++;
    
    // 标记为已释放
    header->magic = 0xDEADBEEF;
    
#if MEM_DEBUG
    // 从链表中移除
    MemBlockHeader** prev = &pool->first_block;
    while (*prev && *prev != header) {
        prev = &(*prev)->next;
    }
    if (*prev == header) {
        *prev = header->next;
    }
#endif
    
    // TODO: 实现内存合并
    
    LeaveCriticalSection(&pool->lock);
}

// ========== 公共API实现 ==========

int mem_init_system(void) {
    for (int i = 0; i < MEM_POOL_COUNT; i++) {
        mem_pool_init(&g_mem_pools[i], (MemPoolType)i);
    }
    
    printf("[内存系统] 玄隐内存池初始化完成\n");
    return 0;
}

void mem_destroy_system(void) {
    size_t leak_bytes = mem_check_leaks();
    if (leak_bytes > 0) {
        printf("[警告] 检测到内存泄露: %zu 字节\n", leak_bytes);
    }
    
    for (int i = 0; i < MEM_POOL_COUNT; i++) {
        mem_pool_destroy(&g_mem_pools[i]);
    }
    
    printf("[内存系统] 内存池已销毁\n");
}

void* mem_alloc(MemPoolType pool_type, size_t size, 
                const char* file, int line, const char* func) {
    if (pool_type < 0 || pool_type >= MEM_POOL_COUNT) {
        return NULL;
    }
    
    return mem_pool_alloc(&g_mem_pools[pool_type], size, file, line, func);
}

void mem_free(void* ptr) {
    if (!ptr) return;
    
    // 找到对应的内存池
    uint8_t* mem = (uint8_t*)ptr - sizeof(MemBlockHeader);
    MemBlockHeader* header = (MemBlockHeader*)mem;
    
    if (header->magic == MEM_MAGIC_NUMBER) {
        if (header->pool_type >= 0 && header->pool_type < MEM_POOL_COUNT) {
            mem_pool_free(&g_mem_pools[header->pool_type], ptr);
        }
    }
}

void* mem_realloc(MemPoolType pool_type, void* ptr, size_t new_size,
                  const char* file, int line, const char* func) {
    if (!ptr) {
        return mem_alloc(pool_type, new_size, file, line, func);
    }
    
    if (new_size == 0) {
        mem_free(ptr);
        return NULL;
    }
    
    // 获取原大小
    uint8_t* mem = (uint8_t*)ptr - sizeof(MemBlockHeader);
    MemBlockHeader* header = (MemBlockHeader*)mem;
    
    if (header->magic != MEM_MAGIC_NUMBER) {
        // 不是我们的内存，回退到系统realloc
        return realloc(ptr, new_size);
    }
    
    // 如果大小相同，直接返回
    if (header->size == new_size) {
        return ptr;
    }
    
    // 分配新内存
    void* new_ptr = mem_alloc(pool_type, new_size, file, line, func);
    if (!new_ptr) {
        return NULL;
    }
    
    // 复制数据
    size_t copy_size = header->size < new_size ? header->size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    
    // 释放旧内存
    mem_free(ptr);
    
    return new_ptr;
}

const MemPoolStats* mem_get_stats(MemPoolType pool_type) {
    if (pool_type < 0 || pool_type >= MEM_POOL_COUNT) {
        return NULL;
    }
    
    return &g_mem_pools[pool_type].stats;
}

void mem_print_report(void) {
    printf("\n========== 玄隐内存报告 ==========\n");
    
    const char* pool_names[] = {
        "全局内存池", "渲染内存池", "世界内存池", 
        "音频内存池", "临时内存池"
    };
    
    for (int i = 0; i < MEM_POOL_COUNT; i++) {
        const MemPoolStats* stats = &g_mem_pools[i].stats;
        printf("%-12s: 使用 %.2fMB / 峰值 %.2fMB | 分配 %zu 次\n",
               pool_names[i],
               stats->current_used / (1024.0f * 1024.0f),
               stats->peak_used / (1024.0f * 1024.0f),
               stats->allocation_count);
    }
    
    printf("==================================\n");
}

size_t mem_check_leaks(void) {
    size_t total_leaks = 0;
    
#if MEM_DEBUG
    printf("\n========== 内存泄露检查 ==========\n");
    
    for (int i = 0; i < MEM_POOL_COUNT; i++) {
        MemoryPool* pool = &g_mem_pools[i];
        MemBlockHeader* block = pool->first_block;
        
        int leak_count = 0;
        while (block) {
            if (block->magic == MEM_MAGIC_NUMBER) {
                printf("[泄露] %s:%d (%s) - %zu 字节\n",
                       block->file, block->line, block->func, block->size);
                total_leaks += block->size;
                leak_count++;
            }
            block = block->next;
        }
        
        if (leak_count > 0) {
            printf("内存池 %d: 发现 %d 处泄露\n", i, leak_count);
        }
    }
    
    printf("总计泄露: %zu 字节\n", total_leaks);
    printf("==================================\n");
#endif
    
    return total_leaks;
}
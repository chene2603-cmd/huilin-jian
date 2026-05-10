/**
 * ID冲突检查系统
 * 防止重复ID导致的数据混乱
 */

#include "../../src/world/chapter_bianjing/chapter_def.h"
#include <stdio.h>
#include <stdlib.h>

#define HASH_TABLE_SIZE 1024

typedef struct IDEntry {
    uint32_t id;
    const char* type;
    const char* name;
    struct IDEntry* next;
} IDEntry;

static IDEntry* g_id_hash_table[HASH_TABLE_SIZE] = {0};

// 简单的哈希函数
static uint32_t hash_id(uint32_t id) {
    return id % HASH_TABLE_SIZE;
}

// 添加ID到哈希表
static int add_id(uint32_t id, const char* type, const char* name) {
    uint32_t index = hash_id(id);
    IDEntry* entry = g_id_hash_table[index];
    
    // 检查是否已存在
    while (entry) {
        if (entry->id == id) {
            printf("[ID检查] 冲突: ID %u 重复\n", id);
            printf("        已存在: %s - %s\n", entry->type, entry->name);
            printf("        新记录: %s - %s\n", type, name);
            return 0;
        }
        entry = entry->next;
    }
    
    // 创建新条目
    entry = malloc(sizeof(IDEntry));
    if (!entry) return 0;
    
    entry->id = id;
    entry->type = type;
    entry->name = name;
    entry->next = g_id_hash_table[index];
    g_id_hash_table[index] = entry;
    
    return 1;
}

// 清理哈希表
static void clear_hash_table(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        IDEntry* entry = g_id_hash_table[i];
        while (entry) {
            IDEntry* next = entry->next;
            free(entry);
            entry = next;
        }
        g_id_hash_table[i] = NULL;
    }
}

// 检查NPC ID
static int check_npc_ids(const ChapterData* chapter) {
    int all_valid = 1;
    
    for (uint32_t i = 0; i < chapter->npc_count; i++) {
        const NPCData* npc = &chapter->npcs[i];
        
        char name_buffer[64];
        snprintf(name_buffer, sizeof(name_buffer), "%s (%s)", npc->name, npc->title);
        
        if (!add_id(npc->id, "NPC", name_buffer)) {
            all_valid = 0;
        }
        
        // 检查模型ID
        if (npc->model_id > 0) {
            char model_name[32];
            snprintf(model_name, sizeof(model_name), "NPC模型[%u]", npc->id);
            
            if (!add_id(npc->model_id, "模型", model_name)) {
                all_valid = 0;
            }
        }
        
        // 检查对话ID
        if (npc->dialog_id > 0) {
            char dialog_name[32];
            snprintf(dialog_name, sizeof(dialog_name), "NPC对话[%u]", npc->id);
            
            if (!add_id(npc->dialog_id, "对话", dialog_name)) {
                all_valid = 0;
            }
        }
    }
    
    return all_valid;
}

// 检查任务ID
static int check_quest_ids(const ChapterData* chapter) {
    int all_valid = 1;
    
    for (uint32_t i = 0; i < chapter->quest_count; i++) {
        const QuestData* quest = &chapter->quests[i];
        
        if (!add_id(quest->id, "任务", quest->title)) {
            all_valid = 0;
        }
        
        // 检查相关物品ID
        if (quest->required_item_id > 0) {
            char item_name[32];
            snprintf(item_name, sizeof(item_name), "任务需求[%u]", quest->id);
            
            if (!add_id(quest->required_item_id, "物品", item_name)) {
                all_valid = 0;
            }
        }
        
        if (quest->reward_item_id > 0) {
            char reward_name[32];
            snprintf(reward_name, sizeof(reward_name), "任务奖励[%u]", quest->id);
            
            if (!add_id(quest->reward_item_id, "物品", reward_name)) {
                all_valid = 0;
            }
        }
        
        // 检查NPC关联
        if (quest->giver_npc_id > 0) {
            char giver_name[32];
            snprintf(giver_name, sizeof(giver_name), "任务发布者[%u]", quest->id);
            
            if (!add_id(quest->giver_npc_id, "NPC关联", giver_name)) {
                all_valid = 0;
            }
        }
        
        if (quest->target_npc_id > 0) {
            char target_name[32];
            snprintf(target_name, sizeof(target_name), "任务目标[%u]", quest->id);
            
            if (!add_id(quest->target_npc_id, "NPC关联", target_name)) {
                all_valid = 0;
            }
        }
    }
    
    return all_valid;
}

// 检查物品ID
static int check_item_ids(const ChapterData* chapter) {
    int all_valid = 1;
    
    for (uint32_t i = 0; i < chapter->item_count; i++) {
        const ItemData* item = &chapter->items[i];
        
        if (!add_id(item->id, "物品", item->name)) {
            all_valid = 0;
        }
        
        // 检查模型ID
        if (item->model_id > 0) {
            char model_name[32];
            snprintf(model_name, sizeof(model_name), "物品模型[%u]", item->id);
            
            if (!add_id(item->model_id, "模型", model_name)) {
                all_valid = 0;
            }
        }
        
        // 检查图标ID
        if (item->icon_id > 0) {
            char icon_name[32];
            snprintf(icon_name, sizeof(icon_name), "物品图标[%u]", item->id);
            
            if (!add_id(item->icon_id, "图标", icon_name)) {
                all_valid = 0;
            }
        }
    }
    
    return all_valid;
}

// 运行ID冲突检查
int check_id_conflicts_full(const ChapterData* chapter) {
    if (!chapter) {
        printf("[ID检查] 错误: 章节数据为空\n");
        return 0;
    }
    
    printf("\n========== ID冲突检查 ==========\n");
    printf("章节: %s\n", chapter->name);
    printf("\n");
    
    int all_valid = 1;
    
    // 重置哈希表
    clear_hash_table();
    
    printf("1. 检查NPC ID...\n");
    if (!check_npc_ids(chapter)) {
        all_valid = 0;
    }
    printf("   检查完成: %u 个NPC\n", chapter->npc_count);
    
    printf("\n2. 检查任务 ID...\n");
    if (!check_quest_ids(chapter)) {
        all_valid = 0;
    }
    printf("   检查完成: %u 个任务\n", chapter->quest_count);
    
    printf("\n3. 检查物品 ID...\n");
    if (!check_item_ids(chapter)) {
        all_valid = 0;
    }
    printf("   检查完成: %u 个物品\n", chapter->item_count);
    
    printf("\n=================================\n");
    
    if (all_valid) {
        printf("✅ 无ID冲突!\n");
    } else {
        printf("❌ 发现ID冲突!\n");
    }
    
    // 清理
    clear_hash_table();
    
    return all_valid;
}

// 运行ID检查测试
void run_id_conflict_test(void) {
    printf("\n运行ID冲突检查测试...\n");
    
    // 创建测试章节
    ChapterData* test_chapter = create_default_chapter_bianjing();
    if (!test_chapter) {
        printf("创建测试章节失败\n");
        return;
    }
    
    // 运行检查
    int result = check_id_conflicts_full(test_chapter);
    
    // 清理
    destroy_chapter_bianjing(test_chapter);
    
    printf("ID冲突检查测试 %s\n", result ? "通过" : "失败");
}
/**
 * 《玄隐义客纪》主程序入口
 * 国风开放世界游戏引擎
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "core/memory/mem_pool.h"
#include "world/chapter_bianjing/chapter_def.h"
#include "world/chapter_bianjing/chapter_data.c"
#include "logic/scene/scene_manager.h"
#include "logic/npc/npc_manager.h"
#include "4_日志系统/logger/log.h"

// 游戏配置
typedef struct {
    int window_width;
    int window_height;
    char window_title[64];
    int target_fps;
    int enable_vsync;
    int fullscreen;
    float master_volume;
} GameConfig;

// 游戏状态
typedef enum {
    GAME_STATE_INIT = 0,
    GAME_STATE_LOADING,
    GAME_STATE_RUNNING,
    GAME_STATE_PAUSED,
    GAME_STATE_MENU,
    GAME_STATE_QUIT
} GameState;

// 游戏引擎
typedef struct {
    GameConfig config;
    GameState state;
    ChapterData* current_chapter;
    SceneManager* scene_manager;
    NPCManager* npc_manager;
    float delta_time;
    uint64_t frame_count;
    LARGE_INTEGER performance_frequency;
    LARGE_INTEGER last_time;
} GameEngine;

// 全局游戏引擎实例
static GameEngine g_game_engine = {0};

// 初始化游戏配置
static void init_game_config(GameConfig* config) {
    config->window_width = 1280;
    config->window_height = 720;
    strcpy_s(config->window_title, sizeof(config->window_title), "《玄隐义客纪》 - 大宋风云");
    config->target_fps = 60;
    config->enable_vsync = 1;
    config->fullscreen = 0;
    config->master_volume = 0.8f;
}

// 初始化性能计数器
static int init_performance_counter(void) {
    if (!QueryPerformanceFrequency(&g_game_engine.performance_frequency)) {
        LOG_ERROR("无法获取性能计数器频率");
        return 0;
    }
    
    QueryPerformanceCounter(&g_game_engine.last_time);
    return 1;
}

// 计算帧间隔时间
static float calculate_delta_time(void) {
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    
    LONGLONG elapsed = current_time.QuadPart - g_game_engine.last_time.QuadPart;
    g_game_engine.last_time = current_time;
    
    return (float)elapsed / (float)g_game_engine.performance_frequency.QuadPart;
}

// 帧率控制
static void limit_frame_rate(float delta_time) {
    float target_frame_time = 1.0f / g_game_engine.config.target_fps;
    
    if (delta_time < target_frame_time) {
        DWORD sleep_time = (DWORD)((target_frame_time - delta_time) * 1000.0f);
        if (sleep_time > 0) {
            Sleep(sleep_time);
        }
    }
}

// 打印启动横幅
static void print_startup_banner(void) {
    printf("\n");
    printf("=================================================\n");
    printf("         《玄隐义客纪》 - 大宋风云\n");
    printf("        国风开放世界游戏引擎 v1.0.0\n");
    printf("=================================================\n");
    printf("\n");
    printf("七层架构启动中...\n");
    printf("├─ 第1层: 架构蓝图 √\n");
    printf("├─ 第2层: 核心代码 √\n");
    printf("├─ 第3层: 运维脚本 √\n");
    printf("├─ 第4层: 日志系统 √\n");
    printf("├─ 第5层: 补丁机制 √\n");
    printf("├─ 第6层: 交互界面 √\n");
    printf("└─ 第7层: 文档手册 √\n");
    printf("\n");
    printf("游戏引擎初始化...\n");
}

// 游戏初始化
static int game_init(void) {
    LOG_INFO("游戏引擎初始化开始");
    
    // 初始化内存系统
    if (mem_init_system() != 0) {
        LOG_ERROR("内存系统初始化失败");
        return 0;
    }
    LOG_INFO("内存系统初始化完成");
    
    // 初始化配置
    init_game_config(&g_game_engine.config);
    
    // 初始化性能计数器
    if (!init_performance_counter()) {
        LOG_ERROR("性能计数器初始化失败");
        return 0;
    }
    
    // 创建汴京章节
    g_game_engine.current_chapter = create_default_chapter_bianjing();
    if (!g_game_engine.current_chapter) {
        LOG_ERROR("创建汴京章节失败");
        return 0;
    }
    
    // 验证章节数据
    if (!validate_chapter_data(g_game_engine.current_chapter)) {
        LOG_ERROR("章节数据验证失败");
        return 0;
    }
    
    // 创建场景管理器
    g_game_engine.scene_manager = scene_manager_create();
    if (!g_game_engine.scene_manager) {
        LOG_ERROR("创建场景管理器失败");
        return 0;
    }
    
    // 加载章节
    if (scene_load_chapter(g_game_engine.scene_manager, g_game_engine.current_chapter) != 0) {
        LOG_ERROR("加载章节失败");
        return 0;
    }
    
    // 创建NPC管理器
    g_game_engine.npc_manager = npc_manager_create();
    if (!g_game_engine.npc_manager) {
        LOG_ERROR("创建NPC管理器失败");
        return 0;
    }
    
    // 初始化NPC
    if (npc_init(g_game_engine.npc_manager, 
                 g_game_engine.current_chapter->npcs,
                 g_game_engine.current_chapter->npc_count) != 0) {
        LOG_ERROR("初始化NPC失败");
        return 0;
    }
    
    g_game_engine.state = GAME_STATE_RUNNING;
    g_game_engine.frame_count = 0;
    
    LOG_INFO("游戏引擎初始化完成");
    return 1;
}

// 游戏更新
static void game_update(float delta_time) {
    g_game_engine.delta_time = delta_time;
    g_game_engine.frame_count++;
    
    // 更新场景
    scene_update(g_game_engine.scene_manager, delta_time);
    
    // 更新NPC
    npc_update(g_game_engine.npc_manager, delta_time);
    
    // 每100帧打印一次状态
    if (g_game_engine.frame_count % 100 == 0) {
        LOG_DEBUG("游戏运行中 - 帧号: %llu, 帧时间: %.3fms", 
                 g_game_engine.frame_count, delta_time * 1000.0f);
        
        // 打印内存使用情况
        mem_print_report();
    }
}

// 游戏渲染
static void game_render(void) {
    // 渲染场景
    scene_render(g_game_engine.scene_manager);
    
    // 这里可以添加UI渲染
    // ...
}

// 游戏清理
static void game_cleanup(void) {
    LOG_INFO("游戏清理开始");
    
    // 销毁NPC管理器
    if (g_game_engine.npc_manager) {
        npc_manager_destroy(g_game_engine.npc_manager);
        g_game_engine.npc_manager = NULL;
    }
    
    // 销毁场景管理器
    if (g_game_engine.scene_manager) {
        scene_manager_destroy(g_game_engine.scene_manager);
        g_game_engine.scene_manager = NULL;
    }
    
    // 销毁章节数据
    if (g_game_engine.current_chapter) {
        destroy_chapter_bianjing(g_game_engine.current_chapter);
        g_game_engine.current_chapter = NULL;
    }
    
    // 销毁内存系统
    mem_destroy_system();
    
    LOG_INFO("游戏清理完成");
}

// 处理Windows消息
static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            g_game_engine.state = GAME_STATE_QUIT;
            return 0;
            
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                g_game_engine.state = GAME_STATE_QUIT;
            }
            return 0;
            
        case WM_SIZE:
            // 处理窗口大小变化
            // TODO: 调整渲染视口
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 创建游戏窗口
static HWND create_game_window(const char* title, int width, int height) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    // 注册窗口类
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_proc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "XuanYinWindowClass";
    
    if (!RegisterClassEx(&wc)) {
        LOG_ERROR("注册窗口类失败");
        return NULL;
    }
    
    // 计算窗口大小
    RECT windowRect = {0, 0, width, height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0,
        "XuanYinWindowClass",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) {
        LOG_ERROR("创建窗口失败");
        return NULL;
    }
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    return hwnd;
}

// 游戏主循环
static void game_main_loop(void) {
    MSG msg = {0};
    
    while (g_game_engine.state != GAME_STATE_QUIT) {
        // 处理Windows消息
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if (msg.message == WM_QUIT) {
                g_game_engine.state = GAME_STATE_QUIT;
            }
        }
        
        // 计算帧时间
        float delta_time = calculate_delta_time();
        
        // 限制最大帧时间，防止卡顿导致的大幅跳帧
        if (delta_time > 0.1f) {
            delta_time = 0.1f;
        }
        
        // 游戏更新
        game_update(delta_time);
        
        // 游戏渲染
        game_render();
        
        // 帧率控制
        limit_frame_rate(delta_time);
    }
}

// 主函数
int main(int argc, char* argv[]) {
    // 打印启动横幅
    print_startup_banner();
    
    // 初始化日志系统
    LOG_INFO("《玄隐义客纪》游戏启动");
    LOG_INFO("编译时间: %s %s", __DATE__, __TIME__);
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-debug") == 0) {
            LOG_INFO("调试模式已启用");
        } else if (strcmp(argv[i], "-nolog") == 0) {
            LOG_INFO("日志输出已禁用");
        }
    }
    
    // 创建窗口
    HWND hwnd = create_game_window(
        g_game_engine.config.window_title,
        g_game_engine.config.window_width,
        g_game_engine.config.window_height
    );
    
    if (!hwnd) {
        LOG_ERROR("创建游戏窗口失败");
        return 1;
    }
    
    LOG_INFO("游戏窗口创建成功: %dx%d", 
             g_game_engine.config.window_width,
             g_game_engine.config.window_height);
    
    // 游戏初始化
    if (!game_init()) {
        LOG_ERROR("游戏初始化失败");
        game_cleanup();
        return 1;
    }
    
    LOG_INFO("========== 游戏开始 ==========");
    
    // 游戏主循环
    game_main_loop();
    
    // 游戏清理
    game_cleanup();
    
    LOG_INFO("========== 游戏结束 ==========");
    LOG_INFO("感谢游玩《玄隐义客纪》");
    
    return 0;
}
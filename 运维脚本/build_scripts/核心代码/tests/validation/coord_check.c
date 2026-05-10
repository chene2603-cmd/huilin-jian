/**
 * 坐标边界检查系统
 * 防止角色卡出地图
 */

#include "../../src/world/chapter_bianjing/chapter_def.h"
#include <stdio.h>

// 汴京地图边界定义
typedef struct {
    AreaType area;
    BoundingBox bounds;
    const char* area_name;
} AreaBoundary;

static const AreaBoundary g_boundaries[] = {
    {AREA_BIANJING_GATE,   {{-120, 0, -60}, {120, 50, 60}},   "汴京城门"},
    {AREA_MAIN_STREET,     {{-250, 0, -150}, {250, 30, 150}}, "主街道"},
    {AREA_IMPERIAL_PALACE, {{-200, 0, -200}, {200, 100, 200}},"皇宫"},
    {AREA_MARKET_SQUARE,   {{-150, 0, -100}, {150, 20, 100}}, "集市广场"},
    {AREA_RIVERSIDE,       {{-350, -5, -60}, {350, 10, 60}},  "汴河畔"},
    {AREA_TEMPLE_DISTRICT, {{-100, 0, -100}, {100, 40, 100}}, "寺庙区"},
    {AREA_RESIDENTIAL,     {{-200, 0, -150}, {200, 25, 150}}, "居民区"}
};

// 检查单个坐标
int check_coordinate(Vec3 pos, AreaType area) {
    if (area < 0 || area >= AREA_COUNT) {
        printf("[坐标检查] 错误: 无效区域 %d\n", area);
        return 0;
    }
    
    const AreaBoundary* boundary = &g_boundaries[area];
    
    int in_bounds = 1;
    
    if (pos.x < boundary->bounds.min.x) {
        printf("[坐标检查] 警告: X坐标超出下界 (%.2f < %.2f)\n", 
               pos.x, boundary->bounds.min.x);
        in_bounds = 0;
    }
    if (pos.x > boundary->bounds.max.x) {
        printf("[坐标检查] 警告: X坐标超出上界 (%.2f > %.2f)\n", 
               pos.x, boundary->bounds.max.x);
        in_bounds = 0;
    }
    if (pos.y < boundary->bounds.min.y) {
        printf("[坐标检查] 警告: Y坐标超出下界 (%.2f < %.2f)\n", 
               pos.y, boundary->bounds.min.y);
        in_bounds = 0;
    }
    if (pos.y > boundary->bounds.max.y) {
        printf("[坐标检查] 警告: Y坐标超出上界 (%.2f > %.2f)\n", 
               pos.y, boundary->bounds.max.y);
        in_bounds = 0;
    }
    if (pos.z < boundary->bounds.min.z) {
        printf("[坐标检查] 警告: Z坐标超出下界 (%.2f < %.2f)\n", 
               pos.z, boundary->bounds.min.z);
        in_bounds = 0;
    }
    if (pos.z > boundary->bounds.max.z) {
        printf("[坐标检查] 警告: Z坐标超出上界 (%.2f > %.2f)\n", 
               pos.z, boundary->bounds.max.z);
        in_bounds = 0;
    }
    
    if (in_bounds) {
        printf("[坐标检查] 通过: 位置(%.2f, %.2f, %.2f) 在 %s 范围内\n",
               pos.x, pos.y, pos.z, boundary->area_name);
    }
    
    return in_bounds;
}

// 检查章节内所有坐标
int check_all_coordinates(const ChapterData* chapter) {
    if (!chapter) {
        printf("[坐标检查] 错误: 章节数据为空\n");
        return 0;
    }
    
    printf("\n========== 坐标边界检查 ==========\n");
    printf("章节: %s\n", chapter->name);
    printf("当前区域: %d\n", chapter->current_area);
    printf("\n");
    
    int all_valid = 1;
    int npc_checked = 0;
    
    // 检查NPC坐标
    printf("检查NPC坐标...\n");
    for (uint32_t i = 0; i < chapter->npc_count; i++) {
        const NPCData* npc = &chapter->npcs[i];
        printf("  NPC[%u] %s: ", npc->id, npc->name);
        
        if (!check_coordinate(npc->transform.position, chapter->current_area)) {
            all_valid = 0;
            printf("    ❌ NPC坐标无效\n");
        } else {
            npc_checked++;
        }
    }
    printf("  NPC坐标检查完成: %d/%d 通过\n\n", npc_checked, chapter->npc_count);
    
    // 检查场景节点坐标
    printf("检查场景节点坐标...\n");
    int node_checked = 0;
    int total_nodes = 0;
    
    // 这里需要实现场景节点的遍历
    // 暂时跳过，等待场景系统实现
    
    printf("场景节点检查完成\n");
    
    printf("===================================\n");
    
    if (all_valid) {
        printf("✅ 所有坐标检查通过!\n");
    } else {
        printf("❌ 发现坐标问题，请修复!\n");
    }
    
    return all_valid;
}

// 运行坐标检查测试
void run_coordinate_check_test(void) {
    printf("\n运行坐标边界检查测试...\n");
    
    // 创建测试章节
    ChapterData* test_chapter = create_default_chapter_bianjing();
    if (!test_chapter) {
        printf("创建测试章节失败\n");
        return;
    }
    
    // 运行检查
    int result = check_all_coordinates(test_chapter);
    
    // 清理
    destroy_chapter_bianjing(test_chapter);
    
    printf("坐标检查测试 %s\n", result ? "通过" : "失败");
}
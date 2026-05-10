/**
 * 汴京章节数据实现
 */

#include "chapter_def.h"
#include <stdio.h>
#include <string.h>
#include "mem_pool.h"

// 汴京边界定义
static const BoundingBox g_area_bounds[AREA_COUNT] = {
    // AREA_BIANJING_GATE
    {{-100.0f, 0.0f, -50.0f}, {100.0f, 50.0f, 50.0f}},
    // AREA_MAIN_STREET
    {{-200.0f, 0.0f, -100.0f}, {200.0f, 30.0f, 100.0f}},
    // AREA_IMPERIAL_PALACE
    {{-150.0f, 0.0f, -150.0f}, {150.0f, 100.0f, 150.0f}},
    // AREA_MARKET_SQUARE
    {{-120.0f, 0.0f, -80.0f}, {120.0f, 20.0f, 80.0f}},
    // AREA_RIVERSIDE
    {{-300.0f, -5.0f, -50.0f}, {300.0f, 10.0f, 50.0f}},
    // AREA_TEMPLE_DISTRICT
    {{-80.0f, 0.0f, -80.0f}, {80.0f, 40.0f, 80.0f}},
    // AREA_RESIDENTIAL
    {{-180.0f, 0.0f, -120.0f}, {180.0f, 25.0f, 120.0f}}
};

// 默认NPC数据
static const NPCData g_default_npcs[] = {
    // 守城士兵
    {
        .id = 10001,
        .name = "张校尉",
        .title = "城门校尉",
        .model_id = 2001,
        .dialog_id = 3001,
        .faction = FACTION_IMPERIAL,
        .transform = {{-10.0f, 0.0f, 0.0f}, {0.0f, 90.0f, 0.0f}, 1.0f},
        .state = NPC_STATE_IDLE,
        .schedule_id = 1,
        .health = 100.0f,
        .max_health = 100.0f,
        .level = 15,
        .flags = 0x01
    },
    // 茶摊老板
    {
        .id = 10002,
        .name = "王掌柜",
        .title = "茶摊老板",
        .model_id = 2002,
        .dialog_id = 3002,
        .faction = FACTION_MERCHANT,
        .transform = {{25.0f, 0.0f, -15.0f}, {0.0f, 180.0f, 0.0f}, 1.0f},
        .state = NPC_STATE_WORKING,
        .schedule_id = 2,
        .health = 50.0f,
        .max_health = 50.0f,
        .level = 5,
        .flags = 0x02
    },
    // 说书先生
    {
        .id = 10003,
        .name = "柳先生",
        .title = "说书人",
        .model_id = 2003,
        .dialog_id = 3003,
        .faction = FACTION_SCHOLAR,
        .transform = {{-5.0f, 0.0f, 20.0f}, {0.0f, 0.0f, 0.0f}, 1.0f},
        .state = NPC_STATE_TALKING,
        .schedule_id = 3,
        .health = 40.0f,
        .max_health = 40.0f,
        .level = 8,
        .flags = 0x04
    }
};

// 默认任务数据
static const QuestData g_default_quests[] = {
    // 入门任务：入城许可
    {
        .id = 50001,
        .title = "入城许可",
        .description = "守城校尉需要你证明身份才能进入汴京城",
        .giver_npc_id = 10001,
        .target_npc_id = 10001,
        .required_item_id = 0,
        .required_count = 0,
        .reward_item_id = 60001,  // 铜钱
        .reward_count = 100,
        .reward_exp = 100,
        .next_quest_id = 50002,
        .state = QUEST_NOT_STARTED,
        .flags = 0x01
    },
    // 支线任务：寻找茶经
    {
        .id = 50002,
        .title = "寻找茶经",
        .description = "茶摊王掌柜的茶经丢失了，帮他寻找",
        .giver_npc_id = 10002,
        .target_npc_id = 10003,
        .required_item_id = 70001,  // 茶经
        .required_count = 1,
        .reward_item_id = 60002,    // 龙井茶
        .reward_count = 5,
        .reward_exp = 200,
        .next_quest_id = 0,
        .state = QUEST_NOT_STARTED,
        .flags = 0x02
    }
};

// 默认物品数据
static const ItemData g_default_items[] = {
    // 货币
    {
        .id = 60001,
        .name = "铜钱",
        .description = "大宋通宝，可购买物品",
        .type = 1,  // 货币
        .sub_type = 1,
        .max_stack = 9999,
        .value = 1,
        .weight = 0.01f,
        .model_id = 4001,
        .icon_id = 5001,
        .flags = 0x01
    },
    {
        .id = 60002,
        .name = "龙井茶",
        .description = "杭州上等龙井，香气扑鼻",
        .type = 2,  // 消耗品
        .sub_type = 1,
        .max_stack = 20,
        .value = 50,
        .weight = 0.1f,
        .model_id = 4002,
        .icon_id = 5002,
        .flags = 0x02
    },
    {
        .id = 70001,
        .name = "茶经",
        .description = "陆羽所著茶经，记载茶道精髓",
        .type = 3,  // 任务物品
        .sub_type = 1,
        .max_stack = 1,
        .value = 0,
        .weight = 0.5f,
        .model_id = 4003,
        .icon_id = 5003,
        .flags = 0x04
    }
};

// 创建默认章节数据
ChapterData* create_default_chapter_bianjing(void) {
    ChapterData* chapter = MEM_NEW(MEM_POOL_WORLD, ChapterData);
    if (!chapter) {
        return NULL;
    }
    
    // 初始化基础信息
    chapter->chapter_id = CHAPTER_BIANJING_ID;
    strcpy_s(chapter->name, sizeof(chapter->name), CHAPTER_NAME);
    chapter->current_area = AREA_BIANJING_GATE;
    chapter->time_of_day = TIME_MORNING;
    chapter->weather = WEATHER_CLEAR;
    chapter->time_scale = 1.0f;
    
    // 创建场景根节点
    chapter->scene_root = MEM_NEW(MEM_POOL_WORLD, SceneNode);
    if (chapter->scene_root) {
        chapter->scene_root->id = 1;
        strcpy_s(chapter->scene_root->name, sizeof(chapter->scene_root->name), "Root");
        chapter->scene_root->transform = (Transform){{0,0,0}, {0,0,0}, 1.0f};
        chapter->scene_root->bounds = (BoundingBox){{-1000,-1000,-1000}, {1000,1000,1000}};
        chapter->scene_root->parent = NULL;
        chapter->scene_root->children = NULL;
        chapter->scene_root->child_count = 0;
        chapter->scene_root->flags = 0x01;
        
        chapter->scene_node_count = 1;
    }
    
    // 复制NPC数据
    chapter->npc_count = sizeof(g_default_npcs) / sizeof(g_default_npcs[0]);
    chapter->npcs = MEM_NEW_ARRAY(MEM_POOL_WORLD, NPCData, chapter->npc_count);
    if (chapter->npcs) {
        memcpy(chapter->npcs, g_default_npcs, sizeof(g_default_npcs));
    }
    
    // 复制任务数据
    chapter->quest_count = sizeof(g_default_quests) / sizeof(g_default_quests[0]);
    chapter->quests = MEM_NEW_ARRAY(MEM_POOL_WORLD, QuestData, chapter->quest_count);
    if (chapter->quests) {
        memcpy(chapter->quests, g_default_quests, sizeof(g_default_quests));
    }
    
    // 复制物品数据
    chapter->item_count = sizeof(g_default_items) / sizeof(g_default_items[0]);
    chapter->items = MEM_NEW_ARRAY(MEM_POOL_WORLD, ItemData, chapter->item_count);
    if (chapter->items) {
        memcpy(chapter->items, g_default_items, sizeof(g_default_items));
    }
    
    chapter->flags = 0x01;
    
    printf("[章节] 汴京章节数据创建完成\n");
    printf("       NPC数量: %u\n", chapter->npc_count);
    printf("       任务数量: %u\n", chapter->quest_count);
    printf("       物品数量: %u\n", chapter->item_count);
    
    return chapter;
}

// 释放章节数据
void destroy_chapter_bianjing(ChapterData* chapter) {
    if (!chapter) return;
    
    if (chapter->scene_root) {
        // 递归释放场景节点
        MEM_FREE(chapter->scene_root);
    }
    
    if (chapter->npcs) {
        for (uint32_t i = 0; i < chapter->npc_count; i++) {
            if (chapter->npcs[i].quest_ids) {
                MEM_FREE(chapter->npcs[i].quest_ids);
            }
        }
        MEM_FREE(chapter->npcs);
    }
    
    if (chapter->quests) {
        MEM_FREE(chapter->quests);
    }
    
    if (chapter->items) {
        MEM_FREE(chapter->items);
    }
    
    MEM_FREE(chapter);
}

// 坐标边界检查
int check_coordinate_bounds(Vec3 pos, AreaType area) {
    if (area < 0 || area >= AREA_COUNT) {
        return 0;  // 无效区域
    }
    
    const BoundingBox* bounds = &g_area_bounds[area];
    
    if (pos.x < bounds->min.x || pos.x > bounds->max.x ||
        pos.y < bounds->min.y || pos.y > bounds->max.y ||
        pos.z < bounds->min.z || pos.z > bounds->max.z) {
        return 0;  // 超出边界
    }
    
    return 1;  // 在边界内
}

// ID冲突检查
int check_id_conflicts(const ChapterData* chapter) {
    if (!chapter) return 0;
    
    // 检查NPC ID冲突
    for (uint32_t i = 0; i < chapter->npc_count; i++) {
        for (uint32_t j = i + 1; j < chapter->npc_count; j++) {
            if (chapter->npcs[i].id == chapter->npcs[j].id) {
                printf("[错误] NPC ID冲突: %u\n", chapter->npcs[i].id);
                return 0;
            }
        }
    }
    
    // 检查任务ID冲突
    for (uint32_t i = 0; i < chapter->quest_count; i++) {
        for (uint32_t j = i + 1; j < chapter->quest_count; j++) {
            if (chapter->quests[i].id == chapter->quests[j].id) {
                printf("[错误] 任务ID冲突: %u\n", chapter->quests[i].id);
                return 0;
            }
        }
    }
    
    // 检查物品ID冲突
    for (uint32_t i = 0; i < chapter->item_count; i++) {
        for (uint32_t j = i + 1; j < chapter->item_count; j++) {
            if (chapter->items[i].id == chapter->items[j].id) {
                printf("[错误] 物品ID冲突: %u\n", chapter->items[i].id);
                return 0;
            }
        }
    }
    
    return 1;  // 无冲突
}

// 数据合法性验证
int validate_chapter_data(const ChapterData* chapter) {
    if (!chapter) {
        printf("[错误] 章节数据为空\n");
        return 0;
    }
    
    if (chapter->chapter_id != CHAPTER_BIANJING_ID) {
        printf("[错误] 章节ID不匹配\n");
        return 0;
    }
    
    if (chapter->current_area >= AREA_COUNT) {
        printf("[错误] 无效区域: %d\n", chapter->current_area);
        return 0;
    }
    
    if (chapter->time_of_day >= TIME_COUNT) {
        printf("[错误] 无效时间: %d\n", chapter->time_of_day);
        return 0;
    }
    
    if (chapter->weather >= WEATHER_COUNT) {
        printf("[错误] 无效天气: %d\n", chapter->weather);
        return 0;
    }
    
    if (!check_id_conflicts(chapter)) {
        return 0;
    }
    
    // 验证NPC数据
    for (uint32_t i = 0; i < chapter->npc_count; i++) {
        const NPCData* npc = &chapter->npcs[i];
        
        if (npc->faction >= FACTION_COUNT) {
            printf("[错误] NPC %u 无效阵营: %d\n", npc->id, npc->faction);
            return 0;
        }
        
        if (npc->state >= NPC_STATE_COUNT) {
            printf("[错误] NPC %u 无效状态: %d\n", npc->id, npc->state);
            return 0;
        }
        
        if (npc->health < 0 || npc->health > npc->max_health) {
            printf("[警告] NPC %u 生命值异常: %.1f/%.1f\n", 
                   npc->id, npc->health, npc->max_health);
        }
    }
    
    // 验证任务数据
    for (uint32_t i = 0; i < chapter->quest_count; i++) {
        const QuestData* quest = &chapter->quests[i];
        
        if (quest->state >= QUEST_COUNT) {
            printf("[错误] 任务 %u 无效状态: %d\n", quest->id, quest->state);
            return 0;
        }
    }
    
    printf("[验证] 汴京章节数据验证通过\n");
    return 1;
}
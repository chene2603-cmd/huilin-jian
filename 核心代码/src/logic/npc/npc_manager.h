/**
 * NPC管理器
 */

#ifndef NPC_MANAGER_H
#define NPC_MANAGER_H

#include "../../world/chapter_bianjing/chapter_def.h"

typedef struct NPCManager NPCManager;

// NPC管理器创建
NPCManager* npc_manager_create(void);

// NPC管理器销毁
void npc_manager_destroy(NPCManager* manager);

// 初始化NPC
int npc_init(NPCManager* manager, NPCData* npcs, uint32_t count);

// 更新NPC AI
void npc_update(NPCManager* manager, float delta_time);

// 查找NPC
NPCData* npc_find_by_id(NPCManager* manager, uint32_t id);

// 查找范围内的NPC
NPCData** npc_find_in_range(NPCManager* manager, Vec3 position, float range, uint32_t* count);

// 设置NPC状态
void npc_set_state(NPCManager* manager, uint32_t npc_id, NPCState state);

// 移动NPC
int npc_move_to(NPCManager* manager, uint32_t npc_id, Vec3 target);

#endif // NPC_MANAGER_H
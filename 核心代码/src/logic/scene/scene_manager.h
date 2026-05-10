/**
 * 场景管理器
 */

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "../../world/chapter_bianjing/chapter_def.h"

typedef struct SceneManager SceneManager;

// 场景管理器创建
SceneManager* scene_manager_create(void);

// 场景管理器销毁
void scene_manager_destroy(SceneManager* manager);

// 加载章节
int scene_load_chapter(SceneManager* manager, ChapterData* chapter);

// 卸载当前章节
void scene_unload_chapter(SceneManager* manager);

// 更新场景
void scene_update(SceneManager* manager, float delta_time);

// 渲染场景
void scene_render(SceneManager* manager);

// 获取当前章节
ChapterData* scene_get_current_chapter(SceneManager* manager);

#endif // SCENE_MANAGER_H
/**
 * 汴京章节定义
 * 大宋都城，第一章主场景
 */

#ifndef CHAPTER_BIANJING_DEF_H
#define CHAPTER_BIANJING_DEF_H

#include "../../core/math/vec3.h"

// 章节ID
#define CHAPTER_BIANJING_ID 1001
#define CHAPTER_NAME "大宋·汴京"

// 区域定义
typedef enum {
    AREA_BIANJING_GATE = 0,     // 汴京城门
    AREA_MAIN_STREET,           // 主街道
    AREA_IMPERIAL_PALACE,       // 皇宫
    AREA_MARKET_SQUARE,         // 集市广场
    AREA_RIVERSIDE,             // 汴河畔
    AREA_TEMPLE_DISTRICT,       // 寺庙区
    AREA_RESIDENTIAL,           // 居民区
    AREA_COUNT
} AreaType;

// 时间定义
typedef enum {
    TIME_DAWN = 0,      // 黎明
    TIME_MORNING,       // 早晨
    TIME_NOON,          // 中午
    TIME_AFTERNOON,     // 下午
    TIME_DUSK,          // 黄昏
    TIME_NIGHT,         // 夜晚
    TIME_MIDNIGHT,      // 午夜
    TIME_COUNT
} TimeOfDay;

// 天气定义
typedef enum {
    WEATHER_CLEAR = 0,  // 晴朗
    WEATHER_CLOUDY,     // 多云
    WEATHER_RAIN,       // 下雨
    WEATHER_SNOW,       // 下雪
    WEATHER_FOG,        // 雾
    WEATHER_WINDY,      // 大风
    WEATHER_COUNT
} WeatherType;

// NPC状态
typedef enum {
    NPC_STATE_IDLE = 0,     // 闲置
    NPC_STATE_WALKING,      // 行走
    NPC_STATE_RUNNING,      // 奔跑
    NPC_STATE_TALKING,      // 对话
    NPC_STATE_FIGHTING,     // 战斗
    NPC_STATE_TRADING,      // 交易
    NPC_STATE_WORKING,      // 工作
    NPC_STATE_RESTING,      // 休息
    NPC_STATE_COUNT
} NPCState;

// 阵营定义
typedef enum {
    FACTION_IMPERIAL = 0,   // 朝廷
    FACTION_MARTIAL,        // 武林
    FACTION_MERCHANT,       // 商贾
    FACTION_SCHOLAR,        // 文人
    FACTION_COMMONER,       // 平民
    FACTION_BANDIT,         // 匪徒
    FACTION_COUNT
} FactionType;

// 任务状态
typedef enum {
    QUEST_NOT_STARTED = 0,  // 未开始
    QUEST_IN_PROGRESS,      // 进行中
    QUEST_COMPLETED,        // 已完成
    QUEST_FAILED,           // 已失败
    QUEST_ABANDONED,        // 已放弃
    QUEST_COUNT
} QuestState;

// 坐标结构
typedef struct {
    Vec3 position;      // 位置
    Vec3 rotation;      // 旋转(欧拉角)
    float scale;        // 缩放
} Transform;

// 边界框
typedef struct {
    Vec3 min;
    Vec3 max;
} BoundingBox;

// 场景节点
typedef struct SceneNode {
    uint32_t id;                // 节点ID
    char name[64];              // 节点名称
    Transform transform;        // 变换
    BoundingBox bounds;         // 边界框
    uint32_t model_id;          // 模型ID
    uint32_t material_id;       // 材质ID
    struct SceneNode* parent;   // 父节点
    struct SceneNode** children;// 子节点数组
    uint32_t child_count;       // 子节点数量
    uint32_t flags;             // 标记位
} SceneNode;

// NPC定义
typedef struct {
    uint32_t id;                // NPC ID
    char name[32];              // 名字
    char title[32];             // 称号
    uint32_t model_id;          // 模型ID
    uint32_t dialog_id;         // 对话ID
    FactionType faction;        // 阵营
    Transform transform;        // 初始位置
    NPCState state;             // 当前状态
    uint32_t schedule_id;       // 日程ID
    uint32_t* quest_ids;        // 相关任务ID数组
    uint32_t quest_count;        // 任务数量
    float health;               // 生命值
    float max_health;           // 最大生命值
    uint32_t level;             // 等级
    uint32_t flags;             // 标记位
} NPCData;

// 任务定义
typedef struct {
    uint32_t id;                // 任务ID
    char title[64];             // 任务标题
    char description[256];      // 任务描述
    uint32_t giver_npc_id;      // 发布者NPC ID
    uint32_t target_npc_id;     // 目标NPC ID
    uint32_t required_item_id;  // 需要物品ID
    uint32_t required_count;    // 需要数量
    uint32_t reward_item_id;    // 奖励物品ID
    uint32_t reward_count;      // 奖励数量
    uint32_t reward_exp;        // 奖励经验
    uint32_t next_quest_id;     // 后续任务ID
    QuestState state;           // 任务状态
    uint32_t flags;             // 标记位
} QuestData;

// 物品定义
typedef struct {
    uint32_t id;                // 物品ID
    char name[32];              // 物品名称
    char description[128];      // 物品描述
    uint32_t type;              // 物品类型
    uint32_t sub_type;          // 物品子类型
    uint32_t max_stack;         // 最大堆叠
    uint32_t value;             // 价值(文钱)
    float weight;               // 重量(斤)
    uint32_t model_id;          // 模型ID
    uint32_t icon_id;           // 图标ID
    uint32_t flags;             // 标记位
} ItemData;

// 章节数据
typedef struct {
    uint32_t chapter_id;        // 章节ID
    char name[32];              // 章节名称
    AreaType current_area;      // 当前区域
    TimeOfDay time_of_day;      // 当前时间
    WeatherType weather;        // 当前天气
    float time_scale;           // 时间缩放
    
    SceneNode* scene_root;      // 场景根节点
    uint32_t scene_node_count;  // 场景节点数量
    
    NPCData* npcs;              // NPC数组
    uint32_t npc_count;         // NPC数量
    
    QuestData* quests;          // 任务数组
    uint32_t quest_count;       // 任务数量
    
    ItemData* items;            // 物品数组
    uint32_t item_count;        // 物品数量
    
    uint32_t flags;             // 章节标记
} ChapterData;

// 坐标边界检查
int check_coordinate_bounds(Vec3 pos, AreaType area);

// ID冲突检查
int check_id_conflicts(const ChapterData* chapter);

// 数据合法性验证
int validate_chapter_data(const ChapterData* chapter);

#endif // CHAPTER_BIANJING_DEF_H
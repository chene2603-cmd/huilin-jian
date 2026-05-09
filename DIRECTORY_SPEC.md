# 目录树规范
# 版本: 1.0.0

## 一、禁止修改项
1. 七层目录名称必须严格保持一致
2. 资源目录结构不得更改
3. 构建输出目录路径固定

## 二、新增模块规范
1. 新增章节: 在 `2_核心代码/src/world/` 下创建 `chapter_章节名`
2. 新增系统: 在 `2_核心代码/src/` 下创建新目录
3. 新增资源: 按类型放入 `resources/` 对应目录

## 三、编译产物规范
1. 调试版本: `build/debug/bin/xuan_yin_yi_ke_debug.exe`
2. 发布版本: `build/release/bin/xuan_yin_yi_ke.exe`
3. 补丁文件: `build/patches/xuan_yin_patch_vX.Y.Z.xyp`

## 四、命名示例
✅ 正确: `chapter_bianjing`, `npc_merchant_li`
❌ 错误: `ChapterBianjing`, `npcMerchantLi`
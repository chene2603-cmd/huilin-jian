@echo off
chcp 65001 > nul
title 《玄隐义客纪》发布版本编译

echo ================================================
echo   《玄隐义客纪》发布版本编译
echo ================================================
echo.

set START_TIME=%time%

echo [1/6] 清理构建目录...
if exist build\release (
    rmdir /s /q build\release
    echo ✓ 清理完成
) else (
    echo ✓ 无需清理
)
mkdir build\release 2>nul
mkdir build\release\bin 2>nul
mkdir build\release\obj 2>nul
mkdir build\release\lib 2>nul
echo.

echo [2/6] 设置编译环境...
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
if %errorlevel% neq 0 (
    call "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if %errorlevel% neq 0 (
    echo ✗ 找不到VC编译环境
    echo   请确认Visual Studio 2019+已安装
    exit /b 1
)
echo ✓ MSVC环境设置完成
echo.

echo [3/6] 编译核心引擎...
set CC=cl
set CFLAGS=/nologo /O2 /MT /W4 /wd4201 /DNDEBUG /D_CRT_SECURE_NO_WARNINGS /I. /I2_核心代码\include /Ithird_party\sdl\include /Ithird_party\glew\include
set LDFLAGS=/nologo /subsystem:windows /ENTRY:mainCRTStartup kernel32.lib user32.lib gdi32.lib winmm.lib opengl32.lib

echo  编译: 内存管理模块...
%CC% %CFLAGS% /c 2_核心代码\src\core\memory\mem_pool.c /Fobuild\release\obj\mem_pool.obj
if %errorlevel% neq 0 exit /b 1

echo  编译: 数学库模块...
%CC% %CFLAGS% /c 2_核心代码\src\core\math\vec3.c /Fobuild\release\obj\vec3.obj
%CC% %CFLAGS% /c 2_核心代码\src\core\math\quaternion.c /Fobuild\release\obj\quaternion.obj
%CC% %CFLAGS% /c 2_核心代码\src\core\math\matrix4x4.c /Fobuild\release\obj\matrix4x4.obj
if %errorlevel% neq 0 exit /b 1

echo  编译: 汴京章节数据...
%CC% %CFLAGS% /c 2_核心代码\src\world\chapter_bianjing\chapter_data.c /Fobuild\release\obj\chapter_data.obj
if %errorlevel% neq 0 exit /b 1

echo  编译: 场景管理器...
%CC% %CFLAGS% /c 2_核心代码\src\logic\scene\scene_manager.c /Fobuild\release\obj\scene_manager.obj
if %errorlevel% neq 0 exit /b 1

echo  编译: NPC管理器...
%CC% %CFLAGS% /c 2_核心代码\src\logic\npc\npc_manager.c /Fobuild\release\obj\npc_manager.obj
if %errorlevel% neq 0 exit /b 1

echo  编译: 日志系统...
%CC% %CFLAGS% /c 4_日志系统\logger\log.c /Fobuild\release\obj\log.obj
if %errorlevel% neq 0 exit /b 1

echo  编译: 主程序...
%CC% %CFLAGS% /c 2_核心代码\src\main.c /Fobuild\release\obj\main.obj
if %errorlevel% neq 0 exit /b 1
echo.

echo [4/6] 链接可执行文件...
echo  链接: 生成主程序...
link /nologo /out:build\release\bin\xuan_yin_yi_ke.exe ^
    build\release\obj\main.obj ^
    build\release\obj\mem_pool.obj ^
    build\release\obj\vec3.obj ^
    build\release\obj\quaternion.obj ^
    build\release\obj\matrix4x4.obj ^
    build\release\obj\chapter_data.obj ^
    build\release\obj\scene_manager.obj ^
    build\release\obj\npc_manager.obj ^
    build\release\obj\log.obj ^
    %LDFLAGS%
if %errorlevel% neq 0 exit /b 1
echo ✓ 可执行文件生成成功
echo.

echo [5/6] 复制运行时文件...
echo  复制: 配置文件...
if not exist build\release\bin\configs mkdir build\release\bin\configs
copy resources\configs\game.cfg build\release\bin\configs\ >nul
copy resources\configs\graphics.cfg build\release\bin\configs\ >nul
copy resources\configs\input.cfg build\release\bin\configs\ >nul

echo  复制: 日志目录...
if not exist build\release\bin\logs mkdir build\release\bin\logs

echo  复制: 启动脚本...
copy 3_运维脚本\build_scripts\run_game.bat build\release\bin\ >nul
echo ✓ 运行时文件复制完成
echo.

echo [6/6] 验证构建结果...
dir build\release\bin\xuan_yin_yi_ke.exe
if %errorlevel% neq 0 (
    echo ✗ 构建失败: 可执行文件不存在
    exit /b 1
)

set END_TIME=%time%
echo ✓ 构建成功!

echo.
echo ================================================
echo   构建完成!
echo   开始时间: %START_TIME%
echo   结束时间: %END_TIME%
echo   输出位置: build\release\bin\xuan_yin_yi_ke.exe
echo ================================================
echo.

echo 运行以下命令启动游戏:
echo   cd build\release\bin
echo   xuan_yin_yi_ke.exe
echo.
pause
@echo off
chcp 65001 > nul
title 《玄隐义客纪》环境检测脚本

echo ================================================
echo   《玄隐义客纪》开发环境检测
echo ================================================
echo.

echo [1/5] 检测操作系统...
ver
echo.

echo [2/5] 检测编译器...
where cl > nul 2>&1
if %errorlevel% equ 0 (
    cl
    echo ✓ MSVC编译器已安装
) else (
    echo ✗ 未找到MSVC编译器
    echo   请安装Visual Studio 2019或更高版本
    exit /b 1
)
echo.

echo [3/5] 检测Make工具...
where make > nul 2>&1
if %errorlevel% equ 0 (
    make --version
    echo ✓ Make工具已安装
) else (
    echo ✗ 未找到Make工具
    echo   请安装MinGW或Cygwin
    exit /b 1
)
echo.

echo [4/5] 检测OpenGL...
where opengl32.dll > nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ OpenGL运行时已安装
) else (
    echo ✗ 未找到OpenGL运行时
    echo   请更新显卡驱动
)
echo.

echo [5/5] 检测磁盘空间...
for /f "tokens=3" %%a in ('dir /-c . ^| find "可用字节"') do set freespace=%%a
set freespace=%freespace:~0,-8%
if %freespace% LSS 10240 (
    echo ✗ 磁盘空间不足: 需要10GB，当前可用 %freespace%GB
    exit /b 1
) else (
    echo ✓ 磁盘空间充足: 可用 %freespace%GB
)
echo.

echo ================================================
echo   环境检测完成，可以开始编译！
echo ================================================
echo.
pause
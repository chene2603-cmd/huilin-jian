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
    rmdir /s /q build
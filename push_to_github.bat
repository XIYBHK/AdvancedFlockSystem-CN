@echo off
chcp 65001 >nul
echo ============================================
echo   AdvancedFlockSystem 汉化版 GitHub 上传脚本
echo ============================================
echo.

REM 检查是否已初始化git
if not exist ".git\" (
    echo [1/5] 正在初始化 Git 仓库...
    git init
) else (
    echo [1/5] Git 仓库已存在，跳过初始化
)

echo.
echo [2/5] 添加文件到 Git...
git add .

echo.
echo [3/5] 创建提交...
git commit -m "初始提交：AdvancedFlockSystem 汉化版" 2>nul || echo 没有变更需要提交

echo.
echo [4/5] 创建 GitHub 仓库...
gh repo create AdvancedFlockSystem-CN --public --source=. --remote=origin --push 2>nul
if errorlevel 1 (
    echo 仓库可能已存在，尝试设置远程仓库...
    git remote remove origin 2>nul
    gh repo create AdvancedFlockSystem-CN --public --source=. --remote=origin --push
)

echo.
echo [5/5] 推送到 GitHub...
git push -u origin main 2>nul || git push -u origin master

echo.
echo ============================================
echo   完成！仓库地址：
echo   https://github.com/XIYBHK/AdvancedFlockSystem-CN
echo ============================================
echo.
pause

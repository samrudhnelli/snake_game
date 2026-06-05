@echo off

g++ snake.cpp

if %errorlevel% equ 0 (
    a.exe
) else (
    echo Sorry the compilation failed:( Please raise an issue.
    pause
)

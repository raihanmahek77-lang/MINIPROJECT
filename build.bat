@echo off
gcc editor.c -o editor.exe
if %ERRORLEVEL% equ 0 (
    echo Compilation successful! Run editor.exe to start.
) else (
    echo Compilation failed.
)

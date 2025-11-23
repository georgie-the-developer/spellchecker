@echo off
setlocal enabledelayedexpansion

REM Get project root = parent directory of this script
set "SCRIPT_DIR=%~dp0"
set "ROOT=%SCRIPT_DIR%.."

REM Normalize
for %%I in ("%ROOT%") do set "ROOT=%%~fI"

set "BUILD_DIR=%ROOT%\bin"
set "OBJ_DIR=%BUILD_DIR%\obj"
set "OUT_EXE=%BUILD_DIR%\main.exe"

if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"

del "%OBJ_DIR%\*.obj" >nul 2>&1
del "%OUT_EXE%" >nul 2>&1

echo Compiling sources...

REM Compile all .cpp files under %ROOT%\src
for /r "%ROOT%\src" %%f in (*.cpp) do (
    echo Compiling %%f
    cl /std:c++17 /W4 /O2 /EHsc /utf-8 /c "%%f" /Fo"%OBJ_DIR%\%%~nf.obj" >nul
)

echo Building object list...

set "OBJ_LIST="
for %%o in ("%OBJ_DIR%\*.obj") do (
    set OBJ_LIST=!OBJ_LIST! %%~fo
)

echo Linking...
link !OBJ_LIST! /OUT:"%OUT_EXE%" >nul

echo Build complete: %OUT_EXE%

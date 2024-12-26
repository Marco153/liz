cls
cl /D_DEBUG /Zi imgui*.obj src/main.cpp /I src/include /I src/include/imgui/docking /I "C:\Program Files (x86)\OpenAL 1.1 SDK\include" /I "C:\Program Files (x86)\libsndfile-1.2.2-win64\include" /MDd /Fe./build/liz.exe /link /libpath:src/lib/  Ole32.lib  glfw3.lib Shlwapi.lib OpenAL32.lib gdi32.lib user32.lib shell32.lib sndfile.lib opengl32.lib /NODEFAULTLIB:MSVCRT
set ORIGINAL_DIR=%cd%
cd /d "E:\projects\WasmGame\lang2\src\lsp"

REM Name of the process to check
set processName=lsp.exe

REM Check if the process is running
tasklist | find /i "%processName%" >nul
if not errorlevel 1 (
    echo Process %processName% is running. Attempting to terminate...
    taskkill /f /im "%processName%"
    if %errorlevel% == 0 (
        echo Successfully terminated %processName%.
    ) else (
        echo Failed to terminate %processName%.
    )
) else (
    echo Process %processName% is not running.
)


call build_intepreter.bat
cd /d %ORIGINAL_DIR%

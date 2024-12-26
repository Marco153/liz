cls
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

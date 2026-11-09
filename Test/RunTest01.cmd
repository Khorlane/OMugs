@echo off
where py >nul 2>&1
if errorlevel 1 (
    python "%~dp0RunSession.py"
) else (
    py -3 "%~dp0RunSession.py"
)
set "test_exit=%errorlevel%"
echo.
pause
exit /b %test_exit%

@echo off
where py >nul 2>&1
if errorlevel 1 (
    python "%~dp0RunSession.py" Test02.txt
) else (
    py -3 "%~dp0RunSession.py" Test02.txt
)
set "test_exit=%errorlevel%"
echo.
pause
exit /b %test_exit%

@echo off
REM Compile the C file
gcc "mem_address.c" -o mem_address.exe

REM Check if compilation was successful
IF %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    pause
    exit /b
)

REM Run the program
mem_address.exe

REM Pause to keep the window open after execution
pause
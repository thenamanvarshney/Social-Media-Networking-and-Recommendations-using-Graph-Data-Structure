@echo off
echo Compiling C++ backend...
cd /d "%~dp0backend-cpp"
g++ -std=c++17 -O2 src/main.cpp src/GraphEngine.cpp -Iinclude -o graph_engine.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] C++ compilation failed.
    pause
    exit /b %ERRORLEVEL%
)
echo [SUCCESS] graph_engine.exe built cleanly!
pause

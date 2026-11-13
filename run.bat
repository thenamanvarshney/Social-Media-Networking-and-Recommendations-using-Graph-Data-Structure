@echo off
cd /d "%~dp0backend-cpp"
if not exist graph_engine.exe (
    echo Compiling graph_engine.exe...
    g++ -std=c++17 -O2 src/main.cpp src/GraphEngine.cpp -Iinclude -o graph_engine.exe
    if %ERRORLEVEL% NEQ 0 (
        echo Compilation failed.
        pause
        exit /b 1
    )
)

cd /d "%~dp0api-bridge"
start "" http://127.0.0.1:8000
python -m uvicorn server:app --host 127.0.0.1 --port 8000 --reload

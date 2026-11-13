# Social Media Graph Engine

**Course Project:** Data Structures (Semester 3)  
**Core Technologies:** C++ (Graph Engine), Python FastAPI (API Bridge), HTML / Tailwind CSS (Frontend)

---

## Overview

A social network graph engine demonstrating graph data structures and recommendation algorithms:
- **Graph Representation:** Undirected graph using an Adjacency List (`std::vector<std::vector<int>>`).
- **User Storage:** Dynamic contiguous array (`std::vector<User>`) with zero-indexed node IDs `[0..N-1]`.
- **Recommendation Algorithm:** Distance-2 triadic closure scored using mutual friends and the **Adamic-Adar Index** ($w = \frac{1}{\ln(\text{degree})}$) to penalize high-degree hubs.
- **Operations:**
  - Change target user perspective.
  - Register new users (`id`, `name`, `handle`).
  - Connect undirected friendship edges between any two nodes.
  - Inspect internal memory representation (array indices, handles, and degrees).
  - Reset to default graph state.

---

## Project Structure

```
.
├── backend-cpp/
│   ├── include/
│   │   ├── User.hpp              # User & Recommendation structs
│   │   └── GraphEngine.hpp       # GraphEngine class definition
│   ├── src/
│   │   ├── GraphEngine.cpp       # Adjacency list & Adamic-Adar algorithm
│   │   └── main.cpp              # CLI driver
│   ├── network_data.txt          # Persistent graph storage
│   └── graph_engine.exe          # Compiled binary
│
├── api-bridge/
│   └── server.py                 # FastAPI server connecting frontend to C++ binary
│
├── frontend/
│   └── index.html                # Clean light-themed web interface
│
├── build.bat                     # Compile C++ backend
└── run.bat                       # Launch application
```

---

## How to Run

### Option 1: Using Batch Script (Windows)
Run `run.bat`:
```cmd
.\run.bat
```
This compiles the C++ backend (if not already compiled), opens `http://127.0.0.1:8000` in your default browser, and starts the FastAPI server.

### Option 2: Manual Execution

1. **Compile C++ Backend:**
   ```bash
   cd backend-cpp
   g++ -std=c++17 -O2 src/main.cpp src/GraphEngine.cpp -Iinclude -o graph_engine.exe
   ```

2. **Start FastAPI Bridge:**
   ```bash
   cd ../api-bridge
   python -m uvicorn server:app --host 127.0.0.1 --port 8000 --reload
   ```

3. **Open Frontend:**
   Navigate to `http://127.0.0.1:8000` in your browser.

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from pydantic import BaseModel
import subprocess
import json
import os
import sys

app = FastAPI(title="Social Graph Engine API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# Robust path detection for backend-cpp directory and executable
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(CURRENT_DIR)
BACKEND_DIR = os.path.join(PROJECT_ROOT, "backend-cpp")
FRONTEND_INDEX = os.path.join(PROJECT_ROOT, "frontend", "index.html")

exe_name = "graph_engine.exe" if sys.platform.startswith("win") else "graph_engine"
BINARY_PATH = os.path.join(BACKEND_DIR, exe_name)

def run_cpp_engine(args: list[str]):
    if not os.path.exists(BINARY_PATH):
        raise HTTPException(
            status_code=500,
            detail=f"C++ binary not found at '{BINARY_PATH}'. Please compile backend-cpp first."
        )
    try:
        result = subprocess.run(
            [BINARY_PATH] + args,
            cwd=BACKEND_DIR,
            capture_output=True,
            text=True,
            check=True
        )
        return json.loads(result.stdout)
    except subprocess.CalledProcessError as e:
        raise HTTPException(
            status_code=500,
            detail=f"C++ engine error: {e.stderr or e.stdout}"
        )
    except json.JSONDecodeError as e:
        raise HTTPException(
            status_code=500,
            detail=f"JSON decoding error from C++ output: {str(e)}"
        )

class UserCreate(BaseModel):
    name: str
    handle: str

class FriendshipCreate(BaseModel):
    u: int
    v: int

@app.get("/")
def serve_index():
    if os.path.exists(FRONTEND_INDEX):
        return FileResponse(FRONTEND_INDEX)
    return {"message": "Social Graph Engine API is running"}

@app.get("/api/state/{user_id}")
def get_state(user_id: int):
    return run_cpp_engine(["state", str(user_id)])

@app.post("/api/user")
def create_user(user: UserCreate):
    return run_cpp_engine(["add_user", user.name, user.handle])

@app.post("/api/friendship")
def create_friendship(edge: FriendshipCreate):
    return run_cpp_engine(["add_edge", str(edge.u), str(edge.v)])

@app.post("/api/reset")
def reset_network():
    return run_cpp_engine(["reset"])

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("server:app", host="127.0.0.1", port=8000, reload=True)

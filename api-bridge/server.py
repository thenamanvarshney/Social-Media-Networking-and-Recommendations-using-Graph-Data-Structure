from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from pydantic import BaseModel
import subprocess
import json
import math
import os
import sys

app = FastAPI(title="Social Graph Engine API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# Robust path detection
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(CURRENT_DIR)
BACKEND_DIR = os.path.join(PROJECT_ROOT, "backend-cpp")
FRONTEND_INDEX = os.path.join(PROJECT_ROOT, "frontend", "index.html")
DATA_FILE = os.path.join(BACKEND_DIR, "network_data.txt")

exe_name = "graph_engine.exe" if sys.platform.startswith("win") else "graph_engine"
BINARY_PATH = os.path.join(BACKEND_DIR, exe_name)

# ----------------- In-Memory Fallback Graph Engine -----------------
# Mirrors the exact C++ GraphEngine logic (useful if Windows Smart App Control blocks unsigned binaries)
class FallbackGraphEngine:
    def __init__(self):
        self.users = []
        self.adj_list = []
        if not self.load_from_file(DATA_FILE):
            self.seed_default_data()
            self.save_to_file(DATA_FILE)

    def edge_exists(self, u: int, v: int) -> bool:
        if 0 <= u < len(self.adj_list):
            return v in self.adj_list[u]
        return False

    def add_user(self, name: str, handle: str) -> int:
        new_id = len(self.users)
        self.users.append({"id": new_id, "name": name, "handle": handle})
        self.adj_list.append([])
        return new_id

    def add_friendship(self, u: int, v: int) -> bool:
        n = len(self.users)
        if u < 0 or u >= n or v < 0 or v >= n or u == v:
            return False
        if self.edge_exists(u, v):
            return False
        self.adj_list[u].append(v)
        self.adj_list[v].append(u)
        return True

    def seed_default_data(self):
        self.users = []
        self.adj_list = []
        self.add_user("Ojas Dongre", "ojas")
        self.add_user("Prajwal Patekar", "prajwal")
        self.add_user("Ved Niturkar", "ved")
        self.add_user("Raj Shinde", "raj")
        self.add_user("Prathamesh Kulkarni", "prathamesh")
        self.add_user("Chinmay Mudgalkar", "chinmay")
        self.add_user("Atharva Joshi", "atharva")

        self.add_friendship(0, 1)
        self.add_friendship(0, 2)
        self.add_friendship(0, 4)
        self.add_friendship(1, 3)
        self.add_friendship(1, 5)
        self.add_friendship(2, 3)
        self.add_friendship(2, 6)
        self.add_friendship(4, 3)

    def load_from_file(self, filename: str) -> bool:
        if not os.path.exists(filename):
            return False
        try:
            with open(filename, "r", encoding="utf-8") as f:
                lines = [line.strip() for line in f if line.strip()]
            if not lines:
                return False
            n = int(lines[0])
            self.users = []
            self.adj_list = [[] for _ in range(n)]
            idx = 1
            for _ in range(n):
                parts = lines[idx].split("|")
                uid = int(parts[0])
                name = parts[1]
                handle = parts[2]
                self.users.append({"id": uid, "name": name, "handle": handle})
                idx += 1
            while idx < len(lines):
                edge_parts = lines[idx].split()
                if len(edge_parts) >= 2:
                    u, v = int(edge_parts[0]), int(edge_parts[1])
                    self.add_friendship(u, v)
                idx += 1
            return True
        except Exception:
            return False

    def save_to_file(self, filename: str) -> bool:
        try:
            with open(filename, "w", encoding="utf-8") as f:
                f.write(f"{len(self.users)}\n")
                for u in self.users:
                    f.write(f"{u['id']}|{u['name']}|{u['handle']}\n")
                for u in range(len(self.adj_list)):
                    for v in self.adj_list[u]:
                        if u < v:
                            f.write(f"{u} {v}\n")
            return True
        except Exception:
            return False

    def get_recommendations(self, target_id: int, top_k: int = 10):
        n = len(self.users)
        if target_id < 0 or target_id >= n:
            return []

        mutual_count = [0] * n
        adamic_adar = [0.0] * n
        is_excluded = [False] * n

        is_excluded[target_id] = True
        for friend in self.adj_list[target_id]:
            is_excluded[friend] = True

        for friend in self.adj_list[target_id]:
            deg = len(self.adj_list[friend])
            weight = (1.0 / math.log(deg)) if deg > 1 else 1.0
            for cand in self.adj_list[friend]:
                if not is_excluded[cand]:
                    mutual_count[cand] += 1
                    adamic_adar[cand] += weight

        results = []
        for i in range(n):
            if mutual_count[i] > 0:
                results.append({
                    "id": i,
                    "name": self.users[i]["name"],
                    "handle": self.users[i]["handle"],
                    "mutualCount": mutual_count[i],
                    "adamicAdar": round(adamic_adar[i], 2)
                })

        results.sort(key=lambda r: (-r["mutualCount"], -r["adamicAdar"]))
        return results[:top_k]

    def export_state_to_json(self, active_user_id: int):
        n = len(self.users)
        if active_user_id < 0 or active_user_id >= n:
            active_user_id = 0 if n > 0 else -1

        users_list = []
        for i, u in enumerate(self.users):
            users_list.append({
                "id": u["id"],
                "name": u["name"],
                "handle": u["handle"],
                "degree": len(self.adj_list[i])
            })

        direct_friends = self.adj_list[active_user_id] if 0 <= active_user_id < n else []

        edges_list = []
        for u in range(len(self.adj_list)):
            for v in self.adj_list[u]:
                if u < v:
                    edges_list.append({"source": u, "target": v})

        recs = self.get_recommendations(active_user_id, 10)

        return {
            "users": users_list,
            "directFriends": direct_friends,
            "edges": edges_list,
            "recommendations": recs
        }

fallback_engine = None

def run_cpp_engine(args: list[str]):
    global fallback_engine
    # Attempt running native C++ binary first
    if os.path.exists(BINARY_PATH):
        try:
            result = subprocess.run(
                [BINARY_PATH] + args,
                cwd=BACKEND_DIR,
                capture_output=True,
                text=True,
                check=True
            )
            return json.loads(result.stdout)
        except (subprocess.CalledProcessError, json.JSONDecodeError):
            pass
        except OSError as e:
            # WinError 4551 or PermissionError (e.g. Windows Smart App Control)
            print(f"[Notice] Native binary blocked by OS policy ({e}). Falling back to internal engine.")

    # Seamless fallback to mirror engine reading the exact same data file
    if fallback_engine is None:
        fallback_engine = FallbackGraphEngine()
    else:
        # Reload latest data file in case it was modified externally
        fallback_engine.load_from_file(DATA_FILE)

    cmd = args[0] if args else "state"
    if cmd == "state":
        uid = int(args[1]) if len(args) > 1 else 0
        return fallback_engine.export_state_to_json(uid)
    elif cmd == "add_user" and len(args) >= 3:
        new_id = fallback_engine.add_user(args[1], args[2])
        fallback_engine.save_to_file(DATA_FILE)
        return fallback_engine.export_state_to_json(new_id)
    elif cmd == "add_edge" and len(args) >= 3:
        u, v = int(args[1]), int(args[2])
        fallback_engine.add_friendship(u, v)
        fallback_engine.save_to_file(DATA_FILE)
        return fallback_engine.export_state_to_json(u)
    elif cmd == "reset":
        fallback_engine.seed_default_data()
        fallback_engine.save_to_file(DATA_FILE)
        return fallback_engine.export_state_to_json(0)

    raise HTTPException(status_code=400, detail="Unknown engine command")

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

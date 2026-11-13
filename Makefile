CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Ibackend-cpp/include

TARGET = backend-cpp/graph_engine
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

SRCS = backend-cpp/src/main.cpp backend-cpp/src/GraphEngine.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TARGET)
	python -m uvicorn api-bridge.server:app --host 127.0.0.1 --port 8000 --reload

clean:
	rm -f $(TARGET)

.PHONY: all run clean

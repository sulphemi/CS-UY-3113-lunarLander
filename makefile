# Source and target
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
OBJS_DEBUG = $(SRCS:.cpp=_debug.o)
TARGET = raylib_app
TARGET_DEBUG = raylib_app_debug

# OS detection (macOS = Darwin, Windows via MinGW = MINGW*)
UNAME_S := $(shell uname -s)

# Default values
CXX = g++
CXXFLAGS = -std=c++23

# Raylib configuration using pkg-config
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

ifeq ($(UNAME_S), Darwin)
    # macOS configuration
    CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
    EXEC_DEBUG = ./$(TARGET_DEBUG)
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    TARGET_DEBUG := $(TARGET_DEBUG).exe
    EXEC = ./$(TARGET)
    EXEC_DEBUG = ./$(TARGET_DEBUG)
else
    # Linux/WSL fallback
    CXXFLAGS +=
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
    EXEC_DEBUG = ./$(TARGET_DEBUG)
endif

.PHONY: all clean run debug

all: $(TARGET)

# Object compilation rules
main.o: airship.cpp
main_debug.o: airship.cpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%_debug.o: %.cpp
	$(CXX) $(CXXFLAGS) -DDEBUG -g -c $< -o $@

# Build rule
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CXX) $(CXXFLAGS) -DDEBUG -g -o $(TARGET_DEBUG) $(OBJS_DEBUG) $(LIBS)

# Clean rule
clean:
	rm -f $(OBJS) $(OBJS_DEBUG) $(TARGET) $(TARGET).exe $(TARGET_DEBUG) $(TARGET_DEBUG).exe

# Run rule
run: $(TARGET)
	$(EXEC)

# Debug rule (builds separate debug binary and runs)
debug: $(TARGET_DEBUG)
	$(EXEC_DEBUG)
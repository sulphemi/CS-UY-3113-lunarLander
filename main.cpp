#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include "raylib.h"
using namespace std;

enum AppStatus : char { RUNNING, TERMINATED, GAMEOVER };

const char* const TITLE = "airship";

// original sprite size
constexpr int ORIGINAL_WIDTH = 640, ORIGINAL_HEIGHT = 360;

// scale factor
constexpr int SCREEN_SCALE = 2;
constexpr Vector2 SCREEN_SCALE_VEC = {.x = SCREEN_SCALE, .y = SCREEN_SCALE};

// target screen params
constexpr int SCREEN_WIDTH = ORIGINAL_WIDTH * SCREEN_SCALE,
              SCREEN_HEIGHT = ORIGINAL_HEIGHT * SCREEN_SCALE;
constexpr int FPS = 60;

AppStatus gAppStatus = RUNNING;
Color gBgColor = {.r = 0x16, .g = 0x16, .b = 0x1d, .a = 0xff};

double lastTickTime;

void init() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  // InitAudioDevice();
  SetTargetFPS(FPS);

  lastTickTime = GetTime();
}

void processInput(double deltaTime) {
  if (WindowShouldClose()) {
    gAppStatus = TERMINATED;
  }
}

void update(double deltaTime) {}

void render(double deltaTime) {
  BeginDrawing();

  ClearBackground(gBgColor);

  EndDrawing();
}

void shutdown() {
  CloseWindow();
}

int main() {
  init();

  while (gAppStatus == RUNNING) {
    double currTime = GetTime();
    double deltaTime = currTime - lastTickTime;
    lastTickTime = currTime;

    processInput(deltaTime);
    update(deltaTime);
    render(deltaTime);
  }

  shutdown();

  return 0;
}

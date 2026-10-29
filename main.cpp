#include <memory>

#include "airship.cpp"
#include "raylib.h"

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

Texture2D gBgTexture;
Texture2D gUITexture;
std::unique_ptr<StaticSprite> gBgSprite;
std::unique_ptr<StaticSprite> gUISprite;

void init() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  // InitAudioDevice();
  SetTargetFPS(FPS);

  lastTickTime = GetTime();

  gBgTexture = LoadTexture("assets/sprites/bg.png");
  gUITexture = LoadTexture("assets/sprites/ui.png");
  gBgSprite = std::make_unique<StaticSprite>(&gBgTexture);
  gUISprite = std::make_unique<StaticSprite>(&gUITexture);
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

  if (gBgSprite) gBgSprite->render();
  if (gUISprite) gUISprite->render();

  EndDrawing();
}

void shutdown() {
  // whats the point of using unique_ptr here if its still necessary to delete
  // it manually lol
  gBgSprite.reset();
  gUISprite.reset();

  UnloadTexture(gBgTexture);
  UnloadTexture(gUITexture);

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

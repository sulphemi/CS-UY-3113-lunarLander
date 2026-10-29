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

constexpr Vector2 AIRSHIP_STARTING_POS = {
    .x = 50.0f * SCREEN_SCALE, .y = 50.0f * SCREEN_SCALE
};

AppStatus gAppStatus = RUNNING;
Color gBgColor = {.r = 0x16, .g = 0x16, .b = 0x1d, .a = 0xff};

double lastTickTime;

Texture2D gBgTexture;
Texture2D gUITexture;
Texture2D gPlayerTexture;
std::unique_ptr<StaticSprite> gBgSprite;
std::unique_ptr<StaticSprite> gUISprite;
std::unique_ptr<Airship> gAirship;

void init() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  // InitAudioDevice();
  SetTargetFPS(FPS);

  lastTickTime = GetTime();

  gBgTexture = LoadTexture("assets/sprites/bg.png");
  gUITexture = LoadTexture("assets/sprites/ui.png");
  gPlayerTexture = LoadTexture("assets/sprites/airship-inverted.png");

  gBgSprite = std::make_unique<StaticSprite>(&gBgTexture);
  gUISprite = std::make_unique<StaticSprite>(&gUITexture);
  gAirship = std::make_unique<Airship>(
      AIRSHIP_STARTING_POS, std::make_unique<StaticSprite>(&gPlayerTexture)
  );
}

void processInput(double deltaTime) {
  if (WindowShouldClose()) {
    gAppStatus = TERMINATED;
  }

  if (gAirship) {
    gAirship->mAcceleration.x = 0.0f;

    if (IsKeyDown(KEY_A)) {
      gAirship->mAcceleration.x -= Airship::ACCELERATION_RATE;
    }
    if (IsKeyDown(KEY_D)) {
      gAirship->mAcceleration.x += Airship::ACCELERATION_RATE;
    }
  }
}

void update(double deltaTime) {
  if (gAirship) gAirship->tick(deltaTime);
}

void render(double deltaTime) {
  BeginDrawing();

  ClearBackground(gBgColor);

  if (gBgSprite) gBgSprite->render();
  if (gAirship) gAirship->render(deltaTime);
  if (gUISprite) gUISprite->render();

  EndDrawing();
}

void shutdown() {
  // whats the point of using unique_ptr here if its still necessary to delete
  // it manually lol
  gAirship.reset();
  gBgSprite.reset();
  gUISprite.reset();

  UnloadTexture(gBgTexture);
  UnloadTexture(gUITexture);
  UnloadTexture(gPlayerTexture);

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

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

constexpr Vector2 X_VELOCITY_DISPLAY_POS = {
    .x = 552.0f * SCREEN_SCALE, .y = 120.0f * SCREEN_SCALE
};

constexpr Vector2 Y_VELOCITY_DISPLAY_POS = {
    .x = 552.0f * SCREEN_SCALE, .y = 147.0f * SCREEN_SCALE
};

constexpr Vector2 ALTITUDE_DISPLAY_POS = {
    .x = 552.0f * SCREEN_SCALE, .y = 91.0f * SCREEN_SCALE
};

constexpr Vector2 AIR_PRESSURE_DISPLAY_POS = {
    .x = 552.0f * SCREEN_SCALE, .y = 176.0f * SCREEN_SCALE
};

constexpr Vector2 FUEL_DISPLAY_POS = {
    .x = 552.0f * SCREEN_SCALE, .y = 203.0f * SCREEN_SCALE
};

constexpr Vector2 ENV_TEMP_DISPLAY_POS = {
    .x = 558.0f * SCREEN_SCALE, .y = 55.0f * SCREEN_SCALE
};

constexpr Vector2 BURNER_TEMP_DISPLAY_POS = {
    .x = 558.0f * SCREEN_SCALE, .y = 28.0f * SCREEN_SCALE
};

AppStatus gAppStatus = RUNNING;
Color gBgColor = {.r = 0x16, .g = 0x16, .b = 0x1d, .a = 0xff};

double lastTickTime;

Texture2D gBgTexture;
Texture2D gUITexture;
Texture2D gNumbersTexture;
Texture2D gPlayerStaticTexture;
Texture2D gPlayerForwardsTexture;
Texture2D gPlayerBackwardsTexture;
std::unique_ptr<StaticSprite> gBgSprite;
std::unique_ptr<StaticSprite> gUISprite;
std::unique_ptr<NumberDisplay> gBurnerTempDisplay;
std::unique_ptr<NumberDisplay> gEnvTempDisplay;
std::unique_ptr<NumberDisplay> gAltitudeDisplay;
std::unique_ptr<NumberDisplay> gXVelDisplay;
std::unique_ptr<NumberDisplay> gYVelDisplay;
std::unique_ptr<NumberDisplay> gAirPressureDisplay;
std::unique_ptr<NumberDisplay> gFuelDisplay;
std::unique_ptr<Airship> gAirship;

void init() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  // InitAudioDevice();
  SetTargetFPS(FPS);

  lastTickTime = GetTime();

  gBgTexture = LoadTexture("assets/sprites/bg.png");
  gUITexture = LoadTexture("assets/sprites/ui.png");
  gNumbersTexture = LoadTexture("assets/sprites/numbers.png");
  gPlayerStaticTexture = LoadTexture("assets/sprites/player-static.png");
  gPlayerForwardsTexture = LoadTexture("assets/sprites/player-forwards.png");
  gPlayerBackwardsTexture = LoadTexture("assets/sprites/player-backwards.png");

  gBgSprite = std::make_unique<StaticSprite>(&gBgTexture);
  gUISprite = std::make_unique<StaticSprite>(&gUITexture);
  gAirship = std::make_unique<Airship>(
      AIRSHIP_STARTING_POS,
      std::make_unique<StaticSprite>(&gPlayerStaticTexture),
      std::make_unique<AnimatedSprite>(&gPlayerForwardsTexture, 2),
      std::make_unique<AnimatedSprite>(&gPlayerBackwardsTexture, 2)
  );

  gAltitudeDisplay = std::make_unique<NumberDisplay>(&gNumbersTexture);
  gXVelDisplay = std::make_unique<NumberDisplay>(&gNumbersTexture);
  gYVelDisplay = std::make_unique<NumberDisplay>(&gNumbersTexture);
  gAirPressureDisplay = std::make_unique<NumberDisplay>(&gNumbersTexture);
  gFuelDisplay = std::make_unique<NumberDisplay>(&gNumbersTexture);
}

void processInput(double deltaTime) {
  if (WindowShouldClose()) {
    gAppStatus = TERMINATED;
  }

  if (gAirship) {
    if (IsKeyPressed(KEY_SPACE)) {
      gAirship->mBurner = !gAirship->mBurner;
    }

    gAirship->mAcceleration.x = 0.0f;

    bool leftPressed = IsKeyDown(KEY_A);
    bool rightPressed = IsKeyDown(KEY_D);

    if (leftPressed && !rightPressed) {
      gAirship->mAcceleration.x -= Airship::ACCELERATION_RATE;
    } else if (rightPressed && !leftPressed) {
      gAirship->mAcceleration.x += Airship::ACCELERATION_RATE;
    }
  }
}

void update(double deltaTime) {
  if (gAirship) {
    gAirship->tick(deltaTime);

    if (gAltitudeDisplay) {
      gAltitudeDisplay->setValue(
          static_cast<int>(std::round((650.0f - gAirship->mPosition.y) * 1.93f))
      );
    }
    if (gXVelDisplay) {
      gXVelDisplay->setValue(
          static_cast<int>(std::round(std::abs(gAirship->mVelocity.x)))
      );
    }
    if (gYVelDisplay) {
      gYVelDisplay->setValue(
          static_cast<int>(std::round(std::abs(gAirship->mVelocity.y)))
      );
    }
    if (gAirPressureDisplay) {
      gAirPressureDisplay->setValue(
          static_cast<int>(std::round(gAirship->mPosition.y * 1.71f))
      );
    }
    if (gFuelDisplay) {
      gFuelDisplay->setValue(static_cast<int>(std::round(gAirship->mFuel)));
    }
  }
}

void render(double deltaTime) {
  BeginDrawing();

  ClearBackground(gBgColor);

  if (gBgSprite) gBgSprite->render();
  if (gAirship) gAirship->render(deltaTime);
  if (gUISprite) gUISprite->render();
  if (gBurnerTempDisplay) gBurnerTempDisplay->render(BURNER_TEMP_DISPLAY_POS);
  if (gEnvTempDisplay) gEnvTempDisplay->render(ENV_TEMP_DISPLAY_POS);
  if (gAltitudeDisplay) gAltitudeDisplay->render(ALTITUDE_DISPLAY_POS);
  if (gXVelDisplay) gXVelDisplay->render(X_VELOCITY_DISPLAY_POS);
  if (gYVelDisplay) gYVelDisplay->render(Y_VELOCITY_DISPLAY_POS);
  if (gAirPressureDisplay)
    gAirPressureDisplay->render(AIR_PRESSURE_DISPLAY_POS);
  if (gFuelDisplay) gFuelDisplay->render(FUEL_DISPLAY_POS);

  EndDrawing();
}

void shutdown() {
  // whats the point of using unique_ptr here if its still necessary to delete
  // it manually lol
  gAirship.reset();
  gFuelDisplay.reset();
  gAirPressureDisplay.reset();
  gAltitudeDisplay.reset();
  gXVelDisplay.reset();
  gYVelDisplay.reset();
  gEnvTempDisplay.reset();
  gBurnerTempDisplay.reset();
  gBgSprite.reset();
  gUISprite.reset();

  UnloadTexture(gBgTexture);
  UnloadTexture(gUITexture);
  UnloadTexture(gNumbersTexture);
  UnloadTexture(gPlayerStaticTexture);
  UnloadTexture(gPlayerForwardsTexture);
  UnloadTexture(gPlayerBackwardsTexture);

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

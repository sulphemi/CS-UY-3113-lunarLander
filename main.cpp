#include <memory>
#include <vector>

#include "airship.cpp"
#include "raylib.h"

enum AppStatus : char { RUNNING, TERMINATED, GAMEOVER, GAMEWIN };

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

constexpr Vector2 AIRSHIP_SPRITE_DIM = {
    .x = 40.0f * SCREEN_SCALE, .y = 23.0f * SCREEN_SCALE
};

constexpr Vector2 AIRSHIP_HITBOX_DIM = {
    .x = 34.0f * SCREEN_SCALE, .y = 20.0f * SCREEN_SCALE
};

constexpr Vector2 AIRSHIP_HITBOX_OFFSET = {
    .x = (AIRSHIP_SPRITE_DIM.x - AIRSHIP_HITBOX_DIM.x) / 2.0f,
    .y = (AIRSHIP_SPRITE_DIM.y - AIRSHIP_HITBOX_DIM.y) / 2.0f
};

constexpr Vector2 ENEMY_1_STARTING_POS = {
    .x = 461.0f * SCREEN_SCALE, .y = 164.0f * SCREEN_SCALE
};

constexpr Vector2 ENEMY_2_STARTING_POS = {
    .x = 238.0f * SCREEN_SCALE, .y = 117.0f * SCREEN_SCALE
};

constexpr Vector2 ENEMY_3_STARTING_POS = {
    .x = 139.0f * SCREEN_SCALE, .y = 243.0f * SCREEN_SCALE
};

constexpr Vector2 ENEMY_SPRITE_DIM = {
    .x = 40.0f * SCREEN_SCALE, .y = 23.0f * SCREEN_SCALE
};

constexpr Vector2 ENEMY_HITBOX_DIM = {
    .x = 34.0f * SCREEN_SCALE, .y = 20.0f * SCREEN_SCALE
};

constexpr Vector2 ENEMY_HITBOX_OFFSET = {
    .x = (ENEMY_SPRITE_DIM.x - ENEMY_HITBOX_DIM.x) / 2.0f,
    .y = (ENEMY_SPRITE_DIM.y - ENEMY_HITBOX_DIM.y) / 2.0f
};

constexpr Vector2 GROUND_1_POS = {
    .x = 0.0f * SCREEN_SCALE, .y = 326.0f * SCREEN_SCALE
};
constexpr Vector2 GROUND_1_DIM = {
    .x = 336.0f * SCREEN_SCALE, .y = 34.0f * SCREEN_SCALE
};

constexpr Vector2 GROUND_2_POS = {
    .x = 335.0f * SCREEN_SCALE, .y = 348.0f * SCREEN_SCALE
};
constexpr Vector2 GROUND_2_DIM = {
    .x = 18.0f * SCREEN_SCALE, .y = 12.0f * SCREEN_SCALE
};

constexpr Vector2 GROUND_3_POS = {
    .x = 405.0f * SCREEN_SCALE, .y = 347.0f * SCREEN_SCALE
};
constexpr Vector2 GROUND_3_DIM = {
    .x = 38.0f * SCREEN_SCALE, .y = 13.0f * SCREEN_SCALE
};

constexpr Vector2 GROUND_4_POS = {
    .x = 442.0f * SCREEN_SCALE, .y = 318.0f * SCREEN_SCALE
};
constexpr Vector2 GROUND_4_DIM = {
    .x = 97.0f * SCREEN_SCALE, .y = 42.0f * SCREEN_SCALE
};

constexpr Vector2 GOAL_POS = {
    .x = 353.0f * SCREEN_SCALE, .y = 343.0f * SCREEN_SCALE
};
constexpr Vector2 GOAL_DIM = {
    .x = 52.0f * SCREEN_SCALE, .y = 17.0f * SCREEN_SCALE
};

constexpr float SAFE_LANDING_SPEED_THRESHOLD = 30.0f;

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
Texture2D gEnemyTexture;
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
std::vector<std::unique_ptr<Entity>> gColliders;

void loseGame() {
#ifdef DEBUG
  std::cout << "LOSE" << std::endl;
#endif
  gAppStatus = GAMEOVER;
}

void winGame() {
#ifdef DEBUG
  std::cout << "WIN" << std::endl;
#endif
  gAppStatus = GAMEWIN;
}

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
  gEnemyTexture = LoadTexture("assets/sprites/enemy.png");

  gBgSprite = std::make_unique<StaticSprite>(&gBgTexture);
  gUISprite = std::make_unique<StaticSprite>(&gUITexture);
  gAirship = std::make_unique<Airship>(
      AIRSHIP_STARTING_POS,
      std::make_unique<StaticSprite>(&gPlayerStaticTexture),
      std::make_unique<AnimatedSprite>(&gPlayerForwardsTexture, 2),
      std::make_unique<AnimatedSprite>(&gPlayerBackwardsTexture, 2),
      std::make_unique<RectangleHitbox>(
          AIRSHIP_HITBOX_DIM, AIRSHIP_HITBOX_OFFSET, PLAYER
      )
  );

  // enemies
  gColliders.push_back(
      std::make_unique<Enemy>(
          ENEMY_1_STARTING_POS,
          100.0f,
          std::make_unique<StaticSprite>(&gEnemyTexture),
          std::make_unique<RectangleHitbox>(
              ENEMY_HITBOX_DIM, ENEMY_HITBOX_OFFSET, BAD
          )
      )
  );
  gColliders.push_back(
      std::make_unique<Enemy>(
          ENEMY_2_STARTING_POS,
          120.0f,
          std::make_unique<StaticSprite>(&gEnemyTexture),
          std::make_unique<RectangleHitbox>(
              ENEMY_HITBOX_DIM, ENEMY_HITBOX_OFFSET, BAD
          )
      )
  );
  gColliders.push_back(
      std::make_unique<Enemy>(
          ENEMY_3_STARTING_POS,
          80.0f,
          std::make_unique<StaticSprite>(&gEnemyTexture),
          std::make_unique<RectangleHitbox>(
              ENEMY_HITBOX_DIM, ENEMY_HITBOX_OFFSET, BAD
          )
      )
  );

  // ground hitboxes
  gColliders.push_back(
      std::make_unique<Entity>(
          GROUND_1_POS,
          nullptr,
          std::make_unique<RectangleHitbox>(
              GROUND_1_DIM, Vector2{.x = 0.0f, .y = 0.0f}, BAD
          )
      )
  );
  gColliders.push_back(
      std::make_unique<Entity>(
          GROUND_2_POS,
          nullptr,
          std::make_unique<RectangleHitbox>(
              GROUND_2_DIM, Vector2{.x = 0.0f, .y = 0.0f}, BAD
          )
      )
  );
  gColliders.push_back(
      std::make_unique<Entity>(
          GROUND_3_POS,
          nullptr,
          std::make_unique<RectangleHitbox>(
              GROUND_3_DIM, Vector2{.x = 0.0f, .y = 0.0f}, BAD
          )
      )
  );
  gColliders.push_back(
      std::make_unique<Entity>(
          GROUND_4_POS,
          nullptr,
          std::make_unique<RectangleHitbox>(
              GROUND_4_DIM, Vector2{.x = 0.0f, .y = 0.0f}, BAD
          )
      )
  );

  // goal hitbox (landing pad)
  gColliders.push_back(
      std::make_unique<Entity>(
          GOAL_POS,
          nullptr,
          std::make_unique<RectangleHitbox>(
              GOAL_DIM, Vector2{.x = 0.0f, .y = 0.0f}, GOAL
          )
      )
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
  for (std::unique_ptr<Entity>& collider : gColliders) {
    collider->tick(deltaTime);
  }

  if (gAirship) {
    gAirship->tick(deltaTime);

    for (std::unique_ptr<Entity>& collider : gColliders) {
      if (collider->isCollidingWith(*gAirship)) {
        switch (collider->mHitbox->mType) {
          case BAD:
            loseGame();
            break;
          case GOAL: {
            float speedSquared = gAirship->mVelocity.x * gAirship->mVelocity.x +
                                 gAirship->mVelocity.y * gAirship->mVelocity.y;
            if (speedSquared <
                SAFE_LANDING_SPEED_THRESHOLD * SAFE_LANDING_SPEED_THRESHOLD) {
              winGame();
            } else {
              loseGame();
            }
            break;
          }
          default:
            break;
        }
      }
    }

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
  for (auto& collider : gColliders) {
    if (collider) collider->render(deltaTime);
  }
  if (gAirship) gAirship->render(deltaTime);
#ifdef DEBUG
  if (gAirship && gAirship->mHitbox) {
    gAirship->mHitbox->render(gAirship->mPosition);
  }
  for (auto& collider : gColliders) {
    if (collider && collider->mHitbox) {
      collider->mHitbox->render(collider->mPosition);
    }
  }
#endif
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
  gColliders.clear();
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
  UnloadTexture(gEnemyTexture);

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

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>

#include "entity.hpp"

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE;

struct StaticSprite : public Renderable {
  StaticSprite(
      const Texture2D* texture, Vector2 origin = {.x = 0.0f, .y = 0.0f}
  ) :
      Renderable(texture, origin) {}

  void render(
      Vector2 renderLocation = {.x = 0.0f, .y = 0.0f},
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  ) const override {
    float textureWidth = static_cast<float>(mTexture->width);
    float textureHeight = static_cast<float>(mTexture->height);

    DrawTexturePro(
        *mTexture,
        Rectangle{
            .x = 0.0f, .y = 0.0f, .width = textureWidth, .height = textureHeight
        },
        Rectangle{
            .x = renderLocation.x,
            .y = renderLocation.y,
            .width = textureWidth * scale.x * SCREEN_SCALE,
            .height = textureHeight * scale.y * SCREEN_SCALE,
        },
        mOrigin,
        0.0f,
        WHITE
    );
  }
};

struct AnimatedSprite : public Renderable {
  int mTotalFrames;
  int mCurrentFrame = 0;
  double mFrameDuration;
  double mElapsedTime = 0.0;
  bool mLoop;

  AnimatedSprite(
      const Texture2D* texture,
      int totalFrames,
      double fps = 12.0,
      bool loop = true,
      Vector2 origin = {.x = 0.0f, .y = 0.0f}
  ) :
      Renderable(texture, origin),
      mTotalFrames(totalFrames),
      mFrameDuration(fps > 0.0 ? 1.0 / fps : 0.0),
      mLoop(loop) {}

  void tick(double deltaTime) override {
    if (mTotalFrames <= 1 || mFrameDuration <= 0.0) {
      return;
    }

    mElapsedTime += deltaTime;
    while (mElapsedTime >= mFrameDuration) {
      mElapsedTime -= mFrameDuration;
      if (mCurrentFrame + 1 < mTotalFrames) {
        mCurrentFrame++;
      } else if (mLoop) {
        mCurrentFrame = 0;
      } else {
        // not looping, stay on frame
        mElapsedTime = 0.0;
        break;
      }
    }
  }

  void render(
      Vector2 renderLocation = {.x = 0.0f, .y = 0.0f},
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  ) const override {
    float frameWidth = static_cast<float>(mTexture->width) / mTotalFrames;
    float frameHeight = static_cast<float>(mTexture->height);

    DrawTexturePro(
        *mTexture,
        Rectangle{
            .x = mCurrentFrame * frameWidth,
            .y = 0.0f,
            .width = frameWidth,
            .height = frameHeight,
        },
        Rectangle{
            .x = renderLocation.x,
            .y = renderLocation.y,
            .width = frameWidth * scale.x * SCREEN_SCALE,
            .height = frameHeight * scale.y * SCREEN_SCALE,
        },
        mOrigin,
        0.0f,
        WHITE
    );
  }
};

struct PhysicsEntity : public Entity {
  Vector2 mVelocity;
  Vector2 mAcceleration;

  PhysicsEntity(
      Vector2 position,
      std::unique_ptr<Renderable> sprite = nullptr,
      std::unique_ptr<Hitbox> hitbox = nullptr,
      Vector2 scale = {.x = 1.0f, .y = 1.0f},
      Vector2 velocity = {.x = 0.0f, .y = 0.0f},
      Vector2 acceleration = {.x = 0.0f, .y = 0.0f}
  ) :
      Entity(position, std::move(sprite), std::move(hitbox), scale),
      mVelocity(velocity),
      mAcceleration(acceleration) {}

  void tick(double deltaTime) override {
    Entity::tick(deltaTime);
    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;
    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;
  }
};

struct Enemy : public PhysicsEntity {
  static constexpr float DEFAULT_SPEED = 60.0f;
  static constexpr float WRAP_X = -50.0f;

  float mSpeed;

  Enemy(
      Vector2 position,
      float speed = DEFAULT_SPEED,
      std::unique_ptr<Renderable> sprite = nullptr,
      std::unique_ptr<Hitbox> hitbox = nullptr,
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  ) :
      PhysicsEntity(
          position,
          std::move(sprite),
          std::move(hitbox),
          scale,
          Vector2{.x = -speed, .y = 0.0f},
          Vector2{.x = 0.0f, .y = 0.0f}
      ),
      mSpeed(speed) {}

  void tick(double deltaTime) override {
    PhysicsEntity::tick(deltaTime);
    if (mPosition.x < WRAP_X * SCREEN_SCALE) {
      mPosition.x = static_cast<float>(SCREEN_WIDTH);
    }
  }
};

struct Airship : public PhysicsEntity {
  static constexpr float ACCELERATION_RATE = 100.0f, MOVEMENT_DECAY = 1.0f;
  static constexpr float GRAVITY = 100.0f;
  static constexpr float RISE_ACCELERATION = 60.0f;
  static constexpr float RISE_SLOWDOWN_DECELERATION = 40.0f;
  static constexpr float BURNER_FUEL_DRAIN_RATE = 1.0f;
  static constexpr float ENGINE_FUEL_DRAIN_RATE = 10.0f;

  bool mBurner = true;
  float mFuel = 1200.0f;
  float mHoverAltitude = 0.0f;

  std::unique_ptr<Renderable> mStaticSprite;
  std::unique_ptr<Renderable> mForwardSprite;
  std::unique_ptr<Renderable> mBackwardSprite;
  Renderable* mCurrentSprite;

  Airship(
      Vector2 position,
      std::unique_ptr<Renderable> staticSprite = nullptr,
      std::unique_ptr<Renderable> forwardSprite = nullptr,
      std::unique_ptr<Renderable> backwardSprite = nullptr,
      std::unique_ptr<Hitbox> hitbox = nullptr
  ) :
      PhysicsEntity(position, nullptr, std::move(hitbox)),
      mHoverAltitude(position.y),
      mStaticSprite(std::move(staticSprite)),
      mForwardSprite(std::move(forwardSprite)),
      mBackwardSprite(std::move(backwardSprite)),
      mCurrentSprite(mStaticSprite.get()) {}

  void updateVerticalMovement(double deltaTime) {
    if (!mBurner) {
      mAcceleration.y = GRAVITY;
      return;
    }

    float distanceToHover = mPosition.y - mHoverAltitude;
    if (distanceToHover <= 0.0f && mVelocity.y <= 0.0f) {
      mPosition.y = mHoverAltitude;
      mVelocity.y = 0.0f;
      mAcceleration.y = 0.0f;
      return;
    }

    // check if the airship needs to start decelerating to come to stop at the
    // hover altitude
    float upwardSpeed = -mVelocity.y;
    float stoppingDistance =
        (upwardSpeed * upwardSpeed) / (2.0f * RISE_SLOWDOWN_DECELERATION);

    if (upwardSpeed > 0.0f && stoppingDistance >= distanceToHover) {
      // slow down smoothly so velocity reaches 0 right at hover altitude
      mAcceleration.y = (upwardSpeed * upwardSpeed) / (2.0f * distanceToHover);
    } else {
      // decelerate downward fall and then speed up upward
      mAcceleration.y = -RISE_ACCELERATION;
    }
  }

  void updateHorizontalMovement(double deltaTime) {
    if (mAcceleration.x == 0.0f) {
      mVelocity.x -= mVelocity.x * MOVEMENT_DECAY * deltaTime;
    }
  }

  void switchSprite() {
    if (mAcceleration.x > 0) {
      mCurrentSprite = mForwardSprite.get();
    } else if (mAcceleration.x < 0) {
      mCurrentSprite = mBackwardSprite.get();
    } else {
      mCurrentSprite = mStaticSprite.get();
    }
  }

  void updateFuel(double deltaTime) {
    if (mFuel <= 0.0f) {
      mFuel = 0.0f;
      return;
    }

    float drain = 0.0f;
    if (mBurner) {
      drain += BURNER_FUEL_DRAIN_RATE;
    }
    drain += ENGINE_FUEL_DRAIN_RATE *
             (std::abs(mAcceleration.x) / ACCELERATION_RATE);

    mFuel = std::max(0.0f, mFuel - drain * static_cast<float>(deltaTime));
  }

  void tick(double deltaTime) override {
    updateVerticalMovement(deltaTime);
    updateHorizontalMovement(deltaTime);
    updateFuel(deltaTime);
    switchSprite();

    PhysicsEntity::tick(deltaTime);

    if (mCurrentSprite) {
      mCurrentSprite->tick(deltaTime);
    }
  }

  void render(double deltaTime) override {
    if (mCurrentSprite) {
      mCurrentSprite->render(mPosition, mScale);
    }
  }
};

struct DigitDisplay : public Renderable {
  static constexpr int TOTAL_DIGITS = 10;

  int mDigit = 0;

  DigitDisplay(
      const Texture2D* texture,
      int digit = 0,
      Vector2 origin = {.x = 0.0f, .y = 0.0f}
  ) :
      Renderable(texture, origin),
      mDigit(std::clamp(digit, 0, 9)) {}

  void setDigit(int digit) { mDigit = std::clamp(digit, 0, 9); }

  void render(
      Vector2 renderLocation = {.x = 0.0f, .y = 0.0f},
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  ) const override {
    if (!mTexture) return;

    float digitWidth = static_cast<float>(mTexture->width) / TOTAL_DIGITS;
    float digitHeight = static_cast<float>(mTexture->height);

    DrawTexturePro(
        *mTexture,
        Rectangle{
            .x = mDigit * digitWidth,
            .y = 0.0f,
            .width = digitWidth,
            .height = digitHeight,
        },
        Rectangle{
            .x = renderLocation.x,
            .y = renderLocation.y,
            .width = digitWidth * scale.x * SCREEN_SCALE,
            .height = digitHeight * scale.y * SCREEN_SCALE,
        },
        mOrigin,
        0.0f,
        WHITE
    );
  }
};

struct NumberDisplay : public Renderable {
  static constexpr int NUM_DIGITS = 4;
  static constexpr int MAX_VALUE = 9999;
  static constexpr float DIGIT_GAP = 1.0f;

  int mValue = 0;
  std::array<DigitDisplay, NUM_DIGITS> mDigits;

  NumberDisplay(
      const Texture2D* texture,
      int value = 0,
      Vector2 origin = {.x = 0.0f, .y = 0.0f}
  ) :
      Renderable(texture, origin),
      mDigits{
          DigitDisplay(texture, 0, origin),
          DigitDisplay(texture, 0, origin),
          DigitDisplay(texture, 0, origin),
          DigitDisplay(texture, 0, origin),
      } {
    setValue(value);
  }

  void setValue(int value) {
    mValue = std::clamp(value, 0, MAX_VALUE);

    mDigits[0].setDigit((mValue / 1000) % 10);
    mDigits[1].setDigit((mValue / 100) % 10);
    mDigits[2].setDigit((mValue / 10) % 10);
    mDigits[3].setDigit(mValue % 10);
  }

  void render(
      Vector2 renderLocation = {.x = 0.0f, .y = 0.0f},
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  ) const override {
    if (!mTexture) return;

    float digitWidth =
        static_cast<float>(mTexture->width) / DigitDisplay::TOTAL_DIGITS;
    float scaledDigitStride = (digitWidth + DIGIT_GAP) * scale.x * SCREEN_SCALE;

    // find first non-zero digit, keeping last digit
    size_t startIndex = NUM_DIGITS - 1;
    for (size_t i = 0; i < NUM_DIGITS - 1; i++) {
      if (mDigits[i].mDigit != 0) {
        startIndex = i;
        break;
      }
    }

    for (size_t i = startIndex; i < NUM_DIGITS; i++) {
      Vector2 digitPos = {
          .x = renderLocation.x + static_cast<float>(i) * scaledDigitStride,
          .y = renderLocation.y,
      };
      mDigits[i].render(digitPos, scale);
    }
  }
};

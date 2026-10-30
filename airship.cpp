#include <algorithm>
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

struct Airship : public PhysicsEntity {
  static constexpr float ACCELERATION_RATE = 100.0f, MOVEMENT_DECAY = 1.0f;
  static constexpr float GRAVITY = 100.0f;
  static constexpr float RISE_ACCELERATION = 60.0f;
  static constexpr float RISE_SLOWDOWN_DECELERATION = 40.0f;

  bool mBurner = true;
  float mFuel = 5000.0f;
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

  void tick(double deltaTime) override {
    updateVerticalMovement(deltaTime);

    if (mAcceleration.x == 0.0f && mVelocity.x != 0.0f) {
      float decayAmount = MOVEMENT_DECAY * deltaTime * mVelocity.x;

      // if velocity is smaller than one step then stop completely
      if (std::fabs(mVelocity.x) <= decayAmount) {
        mPosition.x +=
            (mVelocity.x * (std::fabs(mVelocity.x) / MOVEMENT_DECAY)) * 0.5f;
        mVelocity.x = 0.0f;
      } else {
        float sign = (mVelocity.x > 0.0f) ? -1.0f : 1.0f;
        mVelocity.x += sign * decayAmount;
      }
    }

    std::cout << "Current values:\n"
              << "  Burner: " << (mBurner ? "ON" : "OFF") << "\n"
              << "  Position: (" << mPosition.x << ", " << mPosition.y << ")\n"
              << "  Velocity: (" << mVelocity.x << ", " << mVelocity.y << ")\n"
              << "  Acceleration: (" << mAcceleration.x << ", "
              << mAcceleration.y << ")\n"
              << "  Fuel: " << mFuel << std::endl;

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

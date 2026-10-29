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

  bool mBurner;
  float mFuel;

  Airship(
      Vector2 position,
      std::unique_ptr<Renderable> sprite = nullptr,
      std::unique_ptr<Hitbox> hitbox = nullptr
  ) :
      PhysicsEntity(position, std::move(sprite), std::move(hitbox)) {}

  void tick(double deltaTime) override {
    if (mAcceleration.x == 0.0f && mVelocity.x != 0.0f) {
      float decayAmount = MOVEMENT_DECAY * deltaTime * mVelocity.x;

      // if velocity is smaller than one step then stop completely
      if (std::abs(mVelocity.x) <= decayAmount) {
        mPosition.x +=
            (mVelocity.x * (std::abs(mVelocity.x) / MOVEMENT_DECAY)) * 0.5f;
        mVelocity.x = 0.0f;
      } else {
        float sign = (mVelocity.x > 0.0f) ? -1.0f : 1.0f;
        mVelocity.x += sign * decayAmount;
      }
    }

    PhysicsEntity::tick(deltaTime);
  }
};

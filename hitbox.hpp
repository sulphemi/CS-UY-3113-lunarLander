#pragma once
#include "raylib.h"

enum HitboxType : char { NONE, PLAYER, BAD, GOAL };

struct RectangleHitbox;

struct Hitbox {
  Vector2 mOffset;
  HitboxType mType;

  Hitbox(Vector2 offset = {.x = 0.0f, .y = 0.0f}, HitboxType type = NONE);
  virtual ~Hitbox() = default;
  virtual bool isColliding(
      Vector2 pos, const Hitbox& other, Vector2 otherPos
  ) const = 0;
  virtual bool collidesWithShape(
      Vector2 pos, const RectangleHitbox& other, Vector2 otherPos
  ) const = 0;
  virtual void render(Vector2 pos) const;
};

struct RectangleHitbox : public Hitbox {
  Vector2 mDim;

  RectangleHitbox(
      Vector2 dim,
      Vector2 offset = {.x = 0.0f, .y = 0.0f},
      HitboxType type = NONE
  );

  bool isColliding(
      Vector2 pos, const Hitbox& other, Vector2 otherPos
  ) const override;

  bool collidesWithShape(
      Vector2 pos, const RectangleHitbox& other, Vector2 otherPos
  ) const override;

  void render(Vector2 pos) const override;
};

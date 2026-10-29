#include "hitbox.hpp"

#include <cmath>

Hitbox::Hitbox(Vector2 offset) :
    mOffset(offset) {}

void Hitbox::render(Vector2 pos) const {}

RectangleHitbox::RectangleHitbox(Vector2 dim, Vector2 offset) :
    Hitbox(offset),
    mDim(dim) {}

bool RectangleHitbox::isColliding(
    Vector2 pos, const Hitbox& other, Vector2 otherPos
) const {
  return other.collidesWithShape(otherPos, *this, pos);
}

bool RectangleHitbox::collidesWithShape(
    Vector2 pos, const RectangleHitbox& other, Vector2 otherPos
) const {
  Vector2 myCenter = {
      .x = pos.x + mOffset.x + mDim.x / 2.0f,
      .y = pos.y + mOffset.y + mDim.y / 2.0f
  };
  Vector2 otherCenter = {
      .x = otherPos.x + other.mOffset.x + other.mDim.x / 2.0f,
      .y = otherPos.y + other.mOffset.y + other.mDim.y / 2.0f
  };

  float xDistance =
      std::fabs(myCenter.x - otherCenter.x) - ((mDim.x + other.mDim.x) / 2.0f);
  float yDistance =
      std::fabs(myCenter.y - otherCenter.y) - ((mDim.y + other.mDim.y) / 2.0f);

  return xDistance < 0.0f && yDistance < 0.0f;
}

void RectangleHitbox::render(Vector2 pos) const {
  DrawRectangleLines(
      static_cast<int>(pos.x + mOffset.x),
      static_cast<int>(pos.y + mOffset.y),
      static_cast<int>(mDim.x),
      static_cast<int>(mDim.y),
      WHITE
  );
}

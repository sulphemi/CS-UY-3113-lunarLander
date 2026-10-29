#include "entity.hpp"

#include <cmath>
#include <cstdlib>

/* @brief linearly interpolate between two colors
 * @param a first color
 * @param b second color
 * @param t interpolation factor (0.0 -> a, 1.0 -> b)
 * @return the interpolated color
 */
Color lerpColor(Color a, Color b, float t) {
  return Color{
      .r = static_cast<unsigned char>(a.r + (b.r - a.r) * t),
      .g = static_cast<unsigned char>(a.g + (b.g - a.g) * t),
      .b = static_cast<unsigned char>(a.b + (b.b - a.b) * t),
      .a = static_cast<unsigned char>(a.a + (b.a - a.a) * t),
  };
}

/* @brief clamp a value to a range
 * @param x value to clamp
 * @param min minimum value
 * @param max maximum value
 * @return clamped value
 */
float clamp(float x, float min, float max) {
  return fmax(min, fmin(max, x));
}

/* @brief generate a random float in range [min, max]
 * @param min lower bound
 * @param max upper bound
 * @return a random float in range [min, max]
 */
float randf(float min, float max) {
  float rand01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  return rand01 * (max - min) + min;
}

// ditto, for int in [min, max]
int randi(int min, int max) {
  return rand() % (max - min + 1) + min;
}

Renderable::Renderable(const Texture2D* texture, Vector2 origin) :
    mTexture(texture),
    mOrigin(origin) {}

void Renderable::tick(double deltaTime) {}

Entity::Entity(
    Vector2 position,
    std::unique_ptr<Renderable> sprite,
    std::unique_ptr<Hitbox> hitbox,
    Vector2 scale
) :
    mSprite(std::move(sprite)),
    mHitbox(std::move(hitbox)),
    mPosition(position),
    mScale(scale) {}

void Entity::tick(double deltaTime) {
  if (mSprite) mSprite->tick(deltaTime);
}

void Entity::render(double deltaTime) {
  if (mSprite) mSprite->render(mPosition, mScale);
}

bool Entity::isCollidingWith(const Entity& other) const {
  return mHitbox && other.mHitbox &&
         mHitbox->isColliding(mPosition, *other.mHitbox, other.mPosition);
}

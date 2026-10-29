#pragma once
#include <memory>

#include "hitbox.hpp"
#include "raylib.h"

/* @brief linearly interpolate between two colors
 * @param a first color
 * @param b second color
 * @param t interpolation factor (0.0 -> a, 1.0 -> b)
 * @return the interpolated color
 */
Color lerpColor(Color a, Color b, float t);

/* @brief clamp a value to a range
 * @param x value to clamp
 * @param min minimum value
 * @param max maximum value
 * @return clamped value
 */
float clamp(float x, float min, float max);

/* @brief generate a random float in range [min, max]
 * @param min lower bound
 * @param max upper bound
 * @return a random float in range [min, max]
 */
float randf(float min, float max);

// ditto, for int in [min, max]
int randi(int min, int max);

struct Renderable {
  const Texture2D* mTexture;
  Vector2 mOrigin;
  Renderable(const Texture2D* texture, Vector2 origin = {.x = 0, .y = 0});
  virtual ~Renderable() = default;
  virtual void render(Vector2 renderLocation, Vector2 scale) const = 0;
  virtual void tick(double deltaTime);
};

struct Entity {
  std::unique_ptr<Renderable> mSprite;
  std::unique_ptr<Hitbox> mHitbox;
  Vector2 mPosition;
  Vector2 mScale;

  Entity(
      Vector2 screenCoords,
      std::unique_ptr<Renderable> sprite = nullptr,
      std::unique_ptr<Hitbox> hitbox = nullptr,
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  );

  virtual ~Entity() = default;

  virtual void tick(double deltaTime);

  virtual void render(double deltaTime);

  virtual bool isCollidingWith(const Entity& other) const;
};

#include "entity.hpp"

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE;

struct StaticSprite : public Renderable {
  StaticSprite(const Texture2D* texture) :
      Renderable(texture) {}

  void render(
      Vector2 renderLocation = {.x = 0.0f, .y = 0.0f},
      Vector2 scale = {.x = 1.0f, .y = 1.0f}
  ) const override {
    float textureWidth = static_cast<float>(mTexture->width);
    float textureHeight = static_cast<float>(mTexture->height);

    DrawTexturePro(
        *mTexture,
        Rectangle{
            .x = renderLocation.x,
            .y = renderLocation.y,
            .width = textureWidth,
            .height = textureHeight
        },
        Rectangle{
            .x = 0.0f,
            .y = 0.0f,
            .width = textureWidth * scale.x * SCREEN_SCALE,
            .height = textureHeight * scale.y * SCREEN_SCALE,
        },
        Vector2{.x = 0.0f, .y = 0.0f},
        0.0f,
        WHITE
    );
  }
};
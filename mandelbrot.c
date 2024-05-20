#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int
main(void)
{

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot Set");

  Shader shader = LoadShader(0, "mandelbrot.fs");
  RenderTexture2D texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  float zoom[] = { 6.0, 4.5 };
  float offset[] = { 0.5, 0.5 };

  int zoom_loc = GetShaderLocation(shader, "zoom");
  int offset_loc = GetShaderLocation(shader, "offset");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int h = GetScreenHeight(), w = GetScreenWidth();

    if (IsKeyDown(KEY_D)) {
      offset[0] -= 0.01f;
    } else if (IsKeyDown(KEY_S)) {
      offset[1] -= 0.01f;
    } else if (IsKeyDown(KEY_A)) {
      offset[0] += 0.01f;
    } else if (IsKeyDown(KEY_W)) {
      offset[1] += 0.01f;
    } else if (IsKeyDown(KEY_Z)) {
      zoom[0] *= 0.99f;
      zoom[1] *= 0.99f;
      offset[0] += (offset[0] - 0.5) * 0.01f / 0.99f;
      offset[1] += (offset[1] - 0.5) * 0.01f / 0.99f;
    } else if (IsKeyDown(KEY_X)) {
      zoom[0] *= 1.01f;
      zoom[1] *= 1.01f;
      offset[0] += (0.5 - offset[0]) * 0.01f / 1.01f;
      offset[1] += (0.5 - offset[1]) * 0.01f / 1.01f;
    }

    SetShaderValue(shader, zoom_loc, zoom, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, offset_loc, offset, SHADER_UNIFORM_VEC2);

    BeginTextureMode(texture);
    {
      ClearBackground(BLACK);
      DrawRectangle(0, 0, h, w, BLACK);
    }
    EndTextureMode();

    BeginDrawing();
    {
      ClearBackground(BLACK);

      BeginShaderMode(shader);
      DrawTexture(texture.texture, 0, 0, WHITE);
      EndShaderMode();
    }
    EndDrawing();
  }

  UnloadRenderTexture(texture);
  UnloadShader(shader);

  return 0;
}

#include <math.h>
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int
main(void)
{

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot Set");

  Shader shader = LoadShader(0, "mandelbrot.fs");
  RenderTexture2D texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
  Font font = GetFontDefault();

  float zoom[] = { 6.0, 4.5 };
  float offset[] = { 0.5, 0.5 };
  float interval_length = 0.5;

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
      if (zoom[1] / interval_length < 2.0f) {
        interval_length *= 0.5f;
      }
      offset[0] += (offset[0] - 0.5) * 0.01f / 0.99f;
      offset[1] += (offset[1] - 0.5) * 0.01f / 0.99f;
    } else if (IsKeyDown(KEY_X)) {
      zoom[0] *= 1.01f;
      zoom[1] *= 1.01f;
      if (zoom[1] / interval_length > 4.0f) {
        interval_length *= 2.0f;
      }
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

      int x_axis = offset[0] * w, y_axis = offset[1] * h;
      DrawLine(x_axis, 0, x_axis, h, WHITE);
      DrawLine(0, y_axis, w, y_axis, WHITE);
      float screen_left = -offset[0] * zoom[0];
      float screen_right = (1.0f - offset[0]) * zoom[0];
      for (float t = ceilf(screen_left / interval_length) * interval_length;
           t <= screen_right;
           t += interval_length) {
        int x = (int)((t / zoom[0] + offset[0]) * w);
        const char* buff = TextFormat("%.3g", t);
        Vector2 text_offset = MeasureTextEx(font, buff, 13.0f, 3.0f);
        Vector2 text_pos = { .x = x - text_offset.x * 0.5f,
                             .y = y_axis - text_offset.y };
        if (text_pos.y < text_offset.y) {
          text_pos.y = text_offset.y;
        } else if (text_pos.y + text_offset.y > (float)h) {
          text_pos.y = (float)h - text_offset.y;
        }
        DrawTextEx(font, buff, text_pos, 13.0f, 3.0f, WHITE);
        DrawCircle(x, y_axis, 2, WHITE);
      }
      float screen_top = -offset[1] * zoom[1];
      float screen_bottom = (1.0f - offset[1]) * zoom[1];
      for (float t = ceilf(screen_top / interval_length) * interval_length;
           t <= screen_bottom;
           t += interval_length) {
        int y = (int)((t / zoom[1] + offset[1]) * h);
        const char* buff = TextFormat("%.3g", -t);
        Vector2 text_offset = MeasureTextEx(font, buff, 13.0f, 3.0f);
        Vector2 text_pos = { .x = x_axis - text_offset.x - 4.0f,
                             .y = y - text_offset.y * 0.5f };
        if (text_pos.x < text_offset.x) {
          text_pos.x = 0.0f;
        } else if (text_pos.x + text_offset.x > (float)w) {
          text_pos.x = (float)w - text_offset.x;
        }
        DrawTextEx(font, buff, text_pos, 13.0f, 3.0f, WHITE);
        DrawCircle(x_axis, y, 2, WHITE);
      }
    }
    EndDrawing();
  }

  UnloadRenderTexture(texture);
  UnloadShader(shader);

  return 0;
}

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

char*
load_file(const char* file_path, size_t* file_size)
{
  FILE* file = fopen(file_path, "rb");
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);
  char* file_content = malloc(size);
  fread(file_content, size - 1, 1, file);
  file_content[size - 1] = '\0';
  *file_size = size;
  fclose(file);
  return file_content;
}

int
main(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Grapher");

  size_t shader_size;
  char* shader_text = load_file("grapher.fs", &shader_size);
  size_t shader_write_loc = 197;
  shader_text[shader_write_loc++] = 'y';
  shader_text[shader_write_loc++] = '-';
  shader_text[shader_write_loc++] = 'x';

  printf("%s\n%lld\n", shader_text, shader_size);

  Shader shader = LoadShaderFromMemory(0, shader_text);
  RenderTexture2D texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
  Font font = GetFontDefault();

  Rectangle textbox_rect = { .x = 10.0f,
                             .y = (float)SCREEN_HEIGHT * 0.9f + 10.0f,
                             .width = (float)SCREEN_WIDTH - 20.0f,
                             .height = (float)SCREEN_HEIGHT * 0.1f - 20.0f };
  char textbox_text[30] = { 0 };
  size_t textbox_cap = sizeof(textbox_text) / sizeof(textbox_text[0]);
  size_t cursor_index = 0;
  int typing = false;

  textbox_text[cursor_index++] = 'y';
  textbox_text[cursor_index++] = '-';
  textbox_text[cursor_index++] = 'x';

  float offset[] = { 0.5f, 0.5f };
  float zoom[] = { 4.0f, 3.0f };
  float interval_length = 0.5;

  int zoom_loc = GetShaderLocation(shader, "zoom");
  int offset_loc = GetShaderLocation(shader, "offset");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int w = GetScreenWidth(), h = GetScreenHeight();

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (CheckCollisionPointRec(mouse, textbox_rect)) {
        typing = true;
      } else {
        typing = false;
      }
    }

    int shader_changed = false;
    if (typing) {
      int key = GetKeyPressed();
      if (key == KEY_BACKSPACE) {
        if (cursor_index > 0) {
          textbox_text[--cursor_index] = '\0';
          shader_text[--shader_write_loc] = ' ';
          shader_changed = true;
        }
      } else if (key != 0 && cursor_index < textbox_cap &&
                 key != KEY_LEFT_SHIFT) {
        textbox_text[cursor_index++] = shader_text[shader_write_loc++] =
          GetCharPressed();
        shader_changed = true;
      }
    } else if (IsKeyDown(KEY_D)) {
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

    if (shader_changed) {
      UnloadShader(shader);
      shader = LoadShaderFromMemory(0, shader_text);
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
        const char* buff = TextFormat("%.4g", t);
        Vector2 text_offset = MeasureTextEx(font, buff, 13.0f, 3.0f);
        Vector2 text_pos = { .x = x - text_offset.x * 0.5f,
                             .y = y_axis - text_offset.y };
        if (text_pos.y < text_offset.y) {
          text_pos.y = text_offset.y;
        } else if (text_pos.y + text_offset.y > (float)textbox_rect.y) {
          text_pos.y = (float)textbox_rect.y - text_offset.y;
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
        const char* buff = TextFormat("%.4g", -t);
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

      DrawRectangleRec(textbox_rect, GetColor(0x181818FF));
      // Vector2 text_offset =
      //   MeasureTextEx(font, textbox_text, textbox_rect.height - 4.0f, 3.0f);
      Vector2 text_pos = { .x = textbox_rect.x + 2.0f,
                           .y = textbox_rect.y + 2.0f };
      DrawTextEx(
        font, textbox_text, text_pos, textbox_rect.height - 4.0f, 3.0f, WHITE);
    }
    EndDrawing();
  }

  UnloadRenderTexture(texture);
  UnloadShader(shader);

  free(shader_text);

  return 0;
}

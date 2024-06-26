#include <complex.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ITERATIONS 10
#define MAX_DEGREE 100

#define ROOT_RADIUS 7

const Color root_color[] = { RED, GREEN, BLUE };

typedef struct
{
  size_t degree;
  double complex coeff[MAX_DEGREE + 1];
} Polynomial;

//(x - a)(x - b)(x - c) =
// x ^ 3 - (a + b + c) x ^2 + (ab + bc + ac) x -abc
void
recalculate_coefficients(complex double* roots, Polynomial* p)
{
  p->coeff[1] = -(roots[0] + roots[1] + roots[2]);
  p->coeff[2] = roots[0] * roots[1] + roots[1] * roots[2] + roots[0] * roots[2];
  p->coeff[3] = -roots[0] * roots[1] * roots[2];
}

int
main(void)
{

  double complex roots[] = { -2.0, 0.0, 3.0 + 1.0 * I, 1.0 };
  size_t size_roots = sizeof(roots) / sizeof(roots[0]);
  size_t selected_root = size_roots;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Newton's Method");

  Shader shader = LoadShader(0, "newton.fs");
  RenderTexture2D texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
  Font font = GetFontDefault();

  Polynomial p = { .degree = 3,
                   .coeff = { 1.0, -1.0 - I, -6.0 - 2.0 * I, 0.0 } };

  int iterations_loc = GetShaderLocation(shader, "iterations");

  int zoom_loc = GetShaderLocation(shader, "zoom");
  int offset_loc = GetShaderLocation(shader, "offset");

  int r1_loc = GetShaderLocation(shader, "r1");
  int r2_loc = GetShaderLocation(shader, "r2");
  int r3_loc = GetShaderLocation(shader, "r3");
  int point_loc = GetShaderLocation(shader, "point");

  int c0_loc = GetShaderLocation(shader, "c0");
  int c1_loc = GetShaderLocation(shader, "c1");
  int c2_loc = GetShaderLocation(shader, "c2");
  int c3_loc = GetShaderLocation(shader, "c3");

  int iterations = 0;

  float zoom[] = { 8.0, 6.0 };
  float offset[] = { 0.5, 0.5 };
  float interval_length = 0.5;

  float r1[] = { -2.0, 0.0 };
  float r2[] = { 0.0, 0.0 };
  float r3[] = { 3.0, 1.0 };
  float point[] = { 1.0, 0.0 };

  float c0[] = { 0.0, 0.0 };
  float c1[] = { -6.0, -2.0 };
  float c2[] = { -1.0, -1.0 };
  float c3[] = { 1.0, 0.0 };

  SetShaderValue(shader, iterations_loc, &iterations, SHADER_UNIFORM_INT);

  SetShaderValue(shader, zoom_loc, zoom, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, offset_loc, offset, SHADER_UNIFORM_VEC2);

  SetShaderValue(shader, r1_loc, r1, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, r2_loc, r2, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, r3_loc, r3, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, point_loc, point, SHADER_UNIFORM_VEC2);

  SetShaderValue(shader, c0_loc, c0, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, c1_loc, c1, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, c2_loc, c2, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, c3_loc, c3, SHADER_UNIFORM_VEC2);

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
    } else if (IsKeyPressed(KEY_N)) {
      iterations++;
      SetShaderValue(shader, iterations_loc, &iterations, SHADER_UNIFORM_INT);
    } else if (IsKeyPressed(KEY_P)) {
      iterations--;
      SetShaderValue(shader, iterations_loc, &iterations, SHADER_UNIFORM_INT);
    }
    int mx = GetMouseX();
    int my = GetMouseY();
    complex double z = ((double)mx / w - offset[0]) * zoom[0] -
                       I * (((double)my / h - offset[1]) * zoom[1]);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      for (size_t i = 0; i < size_roots; ++i) {
        if (cabs(z - roots[i]) < 0.1) {
          selected_root = i;
          roots[i] = z;
          break;
        }
      }
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      selected_root = size_roots;
    }
    if (selected_root < size_roots) {
      roots[selected_root] = z;
      recalculate_coefficients(roots, &p);
      r1[0] = creal(roots[0]), r1[1] = cimag(roots[0]);
      r2[0] = creal(roots[1]), r2[1] = cimag(roots[1]);
      r3[0] = creal(roots[2]), r3[1] = cimag(roots[2]);
      point[0] = creal(roots[3]), point[1] = cimag(roots[3]);
      c0[0] = creal(p.coeff[3]), c0[1] = cimag(p.coeff[3]);
      c1[0] = creal(p.coeff[2]), c1[1] = cimag(p.coeff[2]);
      c2[0] = creal(p.coeff[1]), c2[1] = cimag(p.coeff[1]);
      SetShaderValue(shader, r1_loc, r1, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, r2_loc, r2, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, r3_loc, r3, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, point_loc, point, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, c0_loc, c0, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, c1_loc, c1, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, c2_loc, c2, SHADER_UNIFORM_VEC2);
    }
    SetShaderValue(shader, offset_loc, offset, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, zoom_loc, zoom, SHADER_UNIFORM_VEC2);

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
    }
    EndDrawing();
  }

  UnloadRenderTexture(texture);
  UnloadShader(shader);

  return 0;
}

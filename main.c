#include <complex.h>
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

// f(x0) / f'(x0)
double complex
newton_poly_ratio(double complex x, Polynomial* p)
{
  double complex r1 = 0.0;
  for (size_t i = 0; i <= p->degree; ++i) {
    r1 = r1 * x + p->coeff[i];
  }
  double complex r2 = 0.0;
  for (size_t i = 0; i < p->degree; ++i) {
    r2 = r2 * x + (double complex)(p->degree - i) * p->coeff[i];
  }
  return r1 / r2;
}

double complex
newton_method_polynomial(double complex x0, Polynomial* p)
{
  for (size_t i = 0; i < ITERATIONS; ++i) {
    x0 = x0 - newton_poly_ratio(x0, p);
  }
  return x0;
}

// assumes at least one root
size_t
closest_root(double complex z, double complex* roots, size_t size_roots)
{

  size_t min = 0;
  double min_dist = cabs(roots[0] - z);
  double dist;
  for (size_t i = 1; i < size_roots; ++i) {
    if ((dist = cabs(roots[i] - z)) < min_dist) {
      min = i;
      min_dist = dist;
    }
  }
  return min;
}

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

  double complex roots[] = { -2.0, 0.0, 3.0 + 1.0 * I };
  size_t size_roots = sizeof(roots) / sizeof(roots[0]);
  size_t selected_root = size_roots;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Newton's Method");

  Shader shader = LoadShader(0, "newton.fs");

  RenderTexture2D texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  Polynomial p = { .degree = 3,
                   .coeff = { 1.0, -1.0 - I, -6.0 - 2.0 * I, 0.0 } };

  int zoom_loc = GetShaderLocation(shader, "zoom");
  int offset_loc = GetShaderLocation(shader, "offset");

  int r1_loc = GetShaderLocation(shader, "r1");
  int r2_loc = GetShaderLocation(shader, "r2");
  int r3_loc = GetShaderLocation(shader, "r3");

  int c0_loc = GetShaderLocation(shader, "c0");
  int c1_loc = GetShaderLocation(shader, "c1");
  int c2_loc = GetShaderLocation(shader, "c2");
  int c3_loc = GetShaderLocation(shader, "c3");

  float zoom[] = { 8.0, 6.0 };
  float offset[] = { 0.5, 0.5 };

  float r1[] = { -2.0, 0.0 };
  float r2[] = { 0.0, 0.0 };
  float r3[] = { 3.0, 1.0 };

  float c0[] = { 0.0, 0.0 };
  float c1[] = { -6.0, -2.0 };
  float c2[] = { -1.0, -1.0 };
  float c3[] = { 1.0, 0.0 };

  SetShaderValue(shader, zoom_loc, zoom, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, offset_loc, offset, SHADER_UNIFORM_VEC2);

  SetShaderValue(shader, r1_loc, r1, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, r2_loc, r2, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, r3_loc, r3, SHADER_UNIFORM_VEC2);

  SetShaderValue(shader, c0_loc, c0, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, c1_loc, c1, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, c2_loc, c2, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, c3_loc, c3, SHADER_UNIFORM_VEC2);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int h = GetScreenHeight(), w = GetScreenWidth();
    double scale_x = w / zoom[1];
    double scale_y = h / zoom[1];

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
      c0[0] = creal(p.coeff[3]), c0[1] = cimag(p.coeff[3]);
      c1[0] = creal(p.coeff[2]), c1[1] = cimag(p.coeff[2]);
      c2[0] = creal(p.coeff[1]), c2[1] = cimag(p.coeff[1]);
      SetShaderValue(shader, r1_loc, r1, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, r2_loc, r2, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, r3_loc, r3, SHADER_UNIFORM_VEC2);
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

      DrawLine(w / 2, 0, w / 2, h, WHITE);
      DrawLine(0, h / 2, w, h / 2, WHITE);
    }
    EndDrawing();
  }

  UnloadRenderTexture(texture);
  UnloadShader(shader);

  return 0;
}

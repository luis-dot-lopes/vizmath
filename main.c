#include <complex.h>
#include <raylib.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ITERATIONS 10
#define MAX_DEGREE 100

#define MAX_X 8.0
#define MAX_Y 6.0

#define ROOT_RADIUS 7

const Color root_color[] = { RED, GREEN };

typedef struct
{
  size_t degree;
  double complex coeff[MAX_DEGREE + 1];
} Polynomial;

// f'(x0) / f(x0)
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
closest_root(double complex z,
             Polynomial* p,
             double complex* roots,
             size_t size_roots)
{
  double computed_root = newton_method_polynomial(z, p);

  size_t min = 0;
  double min_dist = cabs(roots[0] - computed_root);
  double dist;
  for (size_t i = 1; i < size_roots; ++i) {
    if ((dist = cabs(roots[i] - computed_root)) < min_dist) {
      min = i;
      min_dist = dist;
    }
  }
  return min;
}

int
main(void)
{

  const double complex roots[] = { 2.0 + 3.0 * I, 3.0 + 1.0 * I };
  const size_t size_roots = sizeof(roots) / sizeof(roots[0]);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Newton's Method");

  SetTargetFPS(60);

  Polynomial p = { .degree = 2,
                   .coeff = { 1.0, -5.0 - 4.0 * I, 3.0 + 11.0 * I } };

  int h = GetScreenHeight(), w = GetScreenWidth();
  double scale_x = w / (MAX_X * 2.0);
  double scale_y = h / (MAX_Y * 2.0);

  BeginDrawing();
  {
    DrawLine(0, h / 2, w, h / 2, WHITE);
    DrawLine(w / 2, 0, w / 2, h, WHITE);

    for (int y = -h / 2; y <= h / 2; ++y) {
      for (int x = -w / 2; x <= w / 2; ++x) {
        double complex z = (double)x / scale_x + (double)y / scale_y * I;
        size_t i = closest_root(z, &p, (double complex*)roots, size_roots);
        DrawPixel(x + w / 2, h / 2 - y, root_color[i]);
      }
    }
    for (size_t i = 0; i < size_roots; ++i) {
      DrawCircle(w / 2 + scale_x * creal(roots[i]),
                 h / 2 - scale_y * cimag(roots[i]),
                 ROOT_RADIUS,
                 WHITE);
    }
  }
  EndDrawing();

  printf("HI\n");

  while (!WindowShouldClose()) {
  }

  return 0;
}

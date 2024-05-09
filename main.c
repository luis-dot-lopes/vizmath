#include <complex.h>
#include <raylib.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ITERATIONS 10
#define MAX_DEGREE 100

typedef struct
{
  size_t degree;
  double coeff[MAX_DEGREE + 1];
} Polynomial;

// f'(x0) / f(x0)
double complex
newton_poly_ratio(double complex x, Polynomial* p)
{
  double complex r1 = 0.0;
  for (size_t i = 0; i <= p->degree; ++i) {
    r1 = r1 * x + (double complex)p->coeff[i];
  }
  double complex r2 = 0.0;
  for (size_t i = 0; i < p->degree; ++i) {
    r2 = r2 * x + (double complex)((p->degree - i) * p->coeff[i]);
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

int
main(void)
{

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Newton's Method");

  SetTargetFPS(60);

  Polynomial p = { .degree = 2, .coeff = { 1.0, -5.0, 6.0 } };

  double complex root = newton_method_polynomial(2.7, &p);

  TraceLog(LOG_INFO, "Root calculated %lf", creal(root));

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
  }

  return 0;
}

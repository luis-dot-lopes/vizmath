#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define POINT_RADIUS 10

int
main(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bezier Curves");

  SetTargetFPS(60);

  Vector2 p0 = { .x = 200, .y = 200 }, p1 = { .x = 300, .y = 100 },
          p2 = { .x = 400, .y = 200 };

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(BLACK);

      DrawLineV(p0, p1, WHITE);
      DrawLineV(p1, p2, WHITE);

      DrawCircleV(p0, POINT_RADIUS, WHITE);
      DrawCircleV(p1, POINT_RADIUS, WHITE);
      DrawCircleV(p2, POINT_RADIUS, WHITE);

      // p = (p0 -2p1 + p2)t^2 + (p1 - p2)2t + p2
      Vector2 p = p0;
      for (float t = 0.0f; t <= 1.0f; t += 0.05f) {
        Vector2 p3 = Vector2Lerp(p0, p1, t);
        Vector2 p4 = Vector2Lerp(p1, p2, t);
        Vector2 p5 = Vector2Lerp(p3, p4, t);
        DrawLineV(p, p5, GREEN);
        DrawCircleV(p5, 2, RED);
        p = p5;
      }
    }
    EndDrawing();
  }

  return 0;
}

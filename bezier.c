#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define POINT_RADIUS 8

int
main(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bezier Curves");

  SetTargetFPS(60);

  Vector2 nodes[] = { { .x = 200, .y = 200 },
                      { .x = 300, .y = 100 },
                      { .x = 400, .y = 200 } };
  int node_count = sizeof(nodes) / sizeof(nodes[0]);
  int selected_node = node_count;

  while (!WindowShouldClose()) {

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      for (int i = 0; i < node_count; ++i) {
        if (CheckCollisionPointCircle(mouse, nodes[i], POINT_RADIUS)) {
          selected_node = i;
          break;
        }
      }
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      selected_node = node_count;
    }

    if (selected_node < node_count) {
      nodes[selected_node] = mouse;
    }

    BeginDrawing();
    {
      ClearBackground(BLACK);

      DrawLineV(nodes[0], nodes[1], WHITE);
      DrawLineV(nodes[1], nodes[2], WHITE);

      DrawCircleV(nodes[0], POINT_RADIUS, WHITE);
      DrawCircleV(nodes[1], POINT_RADIUS, WHITE);
      DrawCircleV(nodes[2], POINT_RADIUS, WHITE);

      Vector2 p = nodes[0];
      for (float t = 0.0f; t <= 1.0f; t += 0.05f) {
        Vector2 p3 = Vector2Lerp(nodes[0], nodes[1], t);
        Vector2 p4 = Vector2Lerp(nodes[1], nodes[2], t);
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

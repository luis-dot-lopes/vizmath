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
                      { .x = 250, .y = 100 },
                      { .x = 350, .y = 100 },
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

      for (int i = 0; i < node_count - 1; ++i) {
        DrawLineV(nodes[i], nodes[i + 1], WHITE);
      }
      for (int i = 0; i < node_count; ++i) {
        DrawCircleV(nodes[i], POINT_RADIUS, WHITE);
      }

      Vector2 p = nodes[0];
      for (float t = 0.0f; t <= 1.0f; t += 0.05f) {
        Vector2 lerp_nodes[node_count];
        int lerp_count = node_count;
        for (int i = 0; i < node_count; ++i) {
          lerp_nodes[i] = nodes[i];
        }
        while (lerp_count > 1) {
          for (int j = 0; j < lerp_count - 1; ++j) {
            lerp_nodes[j] = Vector2Lerp(lerp_nodes[j], lerp_nodes[j + 1], t);
          }
          lerp_count--;
        }
        DrawLineV(p, lerp_nodes[0], GREEN);
        DrawCircleV(lerp_nodes[0], 2, RED);
        p = lerp_nodes[0];
      }
    }
    EndDrawing();
  }

  return 0;
}

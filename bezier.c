#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define POINT_RADIUS 8
#define NODE_CAP 10

int
main2(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bezier Curves");

  SetTargetFPS(60);

  Vector2 nodes[NODE_CAP] = { { .x = 200, .y = 200 },
                              { .x = 250, .y = 100 },
                              { .x = 350, .y = 100 },
                              { .x = 400, .y = 200 } };
  int node_count = 4;
  int selected_node = node_count;
  int total_steps = 40;
  int steps = total_steps;
  float step_size = 1.0f / total_steps;

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

    if (IsKeyPressed(KEY_N) && steps < total_steps) {
      steps++;
    } else if (IsKeyPressed(KEY_P) && steps > 0) {
      steps--;
    } else if (IsKeyPressed(KEY_A) && node_count < NODE_CAP) {
      nodes[node_count++] = mouse;
    } else if (IsKeyPressed(KEY_R) && node_count > 0) {
      node_count--;
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
      for (int k = 0; k <= steps; ++k) {
        float t = k * step_size;
        Vector2 lerp_nodes[node_count];
        int lerp_count = node_count;
        for (int i = 0; i < node_count; ++i) {
          lerp_nodes[i] = nodes[i];
        }
        while (lerp_count > 1) {
          for (int j = 0; j < lerp_count - 1; ++j) {
            lerp_nodes[j] = Vector2Lerp(lerp_nodes[j], lerp_nodes[j + 1], t);
            if (k == steps)
              DrawCircleV(lerp_nodes[j], 3, RED);
          }
          lerp_count--;
          if (k == steps) {
            for (int j = 0; j < lerp_count - 1; ++j) {
              DrawLineV(lerp_nodes[j], lerp_nodes[j + 1], BLUE);
            }
          }
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

int
main(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bezier curves - shaders");

  Shader shader = LoadShader(0, "bezier.fs");
  RenderTexture2D texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int h = GetScreenHeight(), w = GetScreenWidth();

    BeginTextureMode(texture);
    {
      ClearBackground(BLACK);
      DrawRectangle(0, 0, w, h, BLACK);
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

  return 0;
}

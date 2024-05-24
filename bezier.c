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

  Vector2 nodes[NODE_CAP] = { { 0.25, 0.33 }, { 0.5, 0.66 }, { 0.75, 0.33 } };
  int node_count = 3;
  int selected_node = node_count;

  float p0[] = { nodes[0].x, nodes[0].y };
  float p1[] = { nodes[1].x, nodes[1].y };
  float p2[] = { nodes[2].x, nodes[2].y };

  int p0_loc = GetShaderLocation(shader, "p0");
  int p1_loc = GetShaderLocation(shader, "p1");
  int p2_loc = GetShaderLocation(shader, "p2");

  SetShaderValue(shader, p0_loc, p0, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, p1_loc, p1, SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, p2_loc, p2, SHADER_UNIFORM_VEC2);

  while (!WindowShouldClose()) {
    int h = GetScreenHeight(), w = GetScreenWidth();

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      for (int i = 0; i < node_count; ++i) {
        Vector2 node_scaled = { .x = nodes[i].x * w,
                                .y = nodes[i].y * h * 0.75 };
        float real_radius = w * 0.01f;
        if (CheckCollisionPointCircle(mouse, node_scaled, real_radius)) {
          selected_node = i;
          break;
        }
      }
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      selected_node = node_count;
    }

    if (selected_node < node_count) {
      nodes[selected_node].x = mouse.x / (float)w;
      nodes[selected_node].y = mouse.y / (float)h * 4.0f / 3.0f;

      p0[0] = nodes[0].x, p0[1] = nodes[0].y;
      p1[0] = nodes[1].x, p1[1] = nodes[1].y;
      p2[0] = nodes[2].x, p2[1] = nodes[2].y;

      SetShaderValue(shader, p0_loc, p0, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, p1_loc, p1, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, p2_loc, p2, SHADER_UNIFORM_VEC2);
    }

    if (IsKeyPressed(KEY_R)) {
      UnloadShader(shader);
      shader = LoadShader(0, "bezier.fs");
    }

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

      for (int i = 0; i < node_count; ++i) {
        DrawCircle(nodes[i].x * w, nodes[i].y * h * 0.75f, 0.01f * w, GREEN);
      }
    }
    EndDrawing();
  }

  UnloadRenderTexture(texture);
  UnloadShader(shader);

  return 0;
}

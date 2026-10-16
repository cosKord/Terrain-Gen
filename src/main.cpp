#include "Heightmap.h"
#include "TerrainConfig.h"
#include "TerrainRenderer.h"
#include "raylib.h"

#include <cstdlib>
#include <ctime>

using namespace std;

int main() {

  const int screenWidth = 1280;
  const int screenHeight = 720;
  InitWindow(screenWidth, screenHeight, "Procedural 3D Terrain Generator");
  SetTargetFPS(60);

  // --- Camera Setup (Free-fly mode) ---
  Camera3D camera{};
  camera.position = {-40.0f, 80.0f, -40.0f};
  camera.target = {128.0f, 0.0f, 128.0f};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // --- Terrain Configuration ---
  TerrainConfig config{};
  srand(static_cast<unsigned>(time(nullptr)));
  config.seed = rand();

  // --- Generate Terrain ---
  Heightmap heightmap;
  heightmap.generate(config);

  TerrainRenderer renderer;
  renderer.buildMesh(heightmap, config);

  bool wireframe = false;

  // --- Main Loop ---
  while (!WindowShouldClose()) {
    // Camera update (free-fly)
    UpdateCamera(&camera, CAMERA_FREE);

    // --- Input Polling ---
    // KEY_R: Randomize seed and regenerate
    if (IsKeyPressed(KEY_R)) {
      config.seed = rand();
      heightmap.generate(config);
      renderer.buildMesh(heightmap, config);
    }

    // KEY_UP / KEY_DOWN: Adjust elevation power
    if (IsKeyPressed(KEY_UP)) {
      config.elevationPower += 0.1f;
      heightmap.generate(config);
      renderer.buildMesh(heightmap, config);
    }
    if (IsKeyPressed(KEY_DOWN)) {
      config.elevationPower -= 0.1f;
      if (config.elevationPower < 0.1f)
        config.elevationPower = 0.1f;
      heightmap.generate(config);
      renderer.buildMesh(heightmap, config);
    }

    // KEY_SPACE: Toggle wireframe
    if (IsKeyPressed(KEY_SPACE)) {
      wireframe = !wireframe;
    }

    // --- Rendering ---
    BeginDrawing();
    ClearBackground({135, 206, 235, 255}); // Sky blue

    BeginMode3D(camera);
    renderer.draw({0.0f, 0.0f, 0.0f}, wireframe);
    DrawGrid(20, 10.0f);
    EndMode3D();

    // HUD
    DrawText("Procedural Terrain Generator", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Seed: %d", config.seed), 10, 40, 16, GRAY);
    DrawText(TextFormat("Elevation Power: %.2f", config.elevationPower), 10, 60,
             16, GRAY);
    DrawText("[R] Regenerate  [UP/DOWN] Elevation  [SPACE] Wireframe", 10, 85,
             14, GRAY);
    DrawFPS(screenWidth - 100, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

#pragma once

#include "Heightmap.h"
#include "TerrainConfig.h"
#include "raylib.h"

// this converts the grid data for gpu
class TerrainRenderer {
public:
  TerrainRenderer() = default;
  ~TerrainRenderer();

  TerrainRenderer(const TerrainRenderer &) = delete;
  TerrainRenderer &operator=(const TerrainRenderer &) = delete;

  TerrainRenderer(TerrainRenderer &&other) noexcept;
  TerrainRenderer &operator=(TerrainRenderer &&other) noexcept;

  void buildMesh(const Heightmap &heightmap, const TerrainConfig &config);

  void draw(Vector3 position, bool wireframe = false) const;
  bool isLoaded() const { return m_loaded; }

private:
  Mesh m_mesh{};
  Model m_model{};
  bool m_loaded = false;

  void unload();
};

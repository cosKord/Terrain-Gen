#pragma once

#include "Heightmap.h"
#include "TerrainConfig.h"
#include "raylib.h"

/// Converts abstract mathematical grid data into GPU-ready buffers.
/// Manages a Raylib Mesh with RAII semantics (UnloadMesh on destruction).
class TerrainRenderer {
public:
  TerrainRenderer() = default;
  ~TerrainRenderer();

  // Non-copyable
  TerrainRenderer(const TerrainRenderer &) = delete;
  TerrainRenderer &operator=(const TerrainRenderer &) = delete;

  // Movable
  TerrainRenderer(TerrainRenderer &&other) noexcept;
  TerrainRenderer &operator=(TerrainRenderer &&other) noexcept;

  /// Build the mesh from a heightmap and terrain configuration.
  /// Allocates vertices, normals, colors, and indices, then uploads to GPU.
  void buildMesh(const Heightmap &heightmap, const TerrainConfig &config);

  /// Draw the terrain mesh at the given position.
  void draw(Vector3 position, bool wireframe = false) const;

  /// Check if a mesh has been built.
  bool isLoaded() const { return m_loaded; }

private:
  Mesh m_mesh{};
  Model m_model{};
  bool m_loaded = false;

  /// Release GPU resources.
  void unload();
};

#include "TerrainRenderer.h"
#include "BiomeColor.h"

#include <cmath>
#include <cstring>
#include <utility>

using namespace std;

TerrainRenderer::~TerrainRenderer() { unload(); }

TerrainRenderer::TerrainRenderer(TerrainRenderer &&other) noexcept
    : m_mesh(other.m_mesh), m_model(other.m_model), m_loaded(other.m_loaded) {
  other.m_loaded = false;
  memset(&other.m_mesh, 0, sizeof(Mesh));
  memset(&other.m_model, 0, sizeof(Model));
}

TerrainRenderer &TerrainRenderer::operator=(TerrainRenderer &&other) noexcept {
  if (this != &other) {
    unload();
    m_mesh = other.m_mesh;
    m_model = other.m_model;
    m_loaded = other.m_loaded;
    other.m_loaded = false;
    memset(&other.m_mesh, 0, sizeof(Mesh));
    memset(&other.m_model, 0, sizeof(Model));
  }
  return *this;
}

void TerrainRenderer::unload() {
  if (m_loaded) {
    UnloadModel(m_model); // UnloadModel also unloads meshes
    m_loaded = false;
    memset(&m_mesh, 0, sizeof(Mesh));
    memset(&m_model, 0, sizeof(Model));
  }
}

void TerrainRenderer::buildMesh(const Heightmap &heightmap,
                                const TerrainConfig &config) {

  unload();

  int width = heightmap.getWidth();
  int depth = heightmap.getDepth();
  int vertexCount = width * depth;
  int triangleCount = (width - 1) * (depth - 1) * 2;

  // --- Allocate Mesh ---
  m_mesh = {};
  m_mesh.vertexCount = vertexCount;
  m_mesh.triangleCount = triangleCount;

  m_mesh.vertices =
      static_cast<float *>(MemAlloc(sizeof(float) * 3 * vertexCount));
  m_mesh.normals =
      static_cast<float *>(MemAlloc(sizeof(float) * 3 * vertexCount));
  m_mesh.colors = static_cast<unsigned char *>(
      MemAlloc(sizeof(unsigned char) * 4 * vertexCount));
  m_mesh.indices = static_cast<unsigned short *>(
      MemAlloc(sizeof(unsigned short) * 3 * triangleCount));

  // --- Populate Vertices ---
  for (int z = 0; z < depth; ++z) {
    for (int x = 0; x < width; ++x) {
      int idx = z * width + x;
      m_mesh.vertices[idx * 3 + 0] = static_cast<float>(x) * config.gridSpacing;
      m_mesh.vertices[idx * 3 + 1] = heightmap.getHeight(x, z);
      m_mesh.vertices[idx * 3 + 2] = static_cast<float>(z) * config.gridSpacing;
    }
  }

  // --- Compute Normals via Finite Differences ---
  // N = normalize( h(x-1,z) - h(x+1,z), 2*gridSpacing, h(x,z-1) - h(x,z+1) )
  for (int z = 0; z < depth; ++z) {
    for (int x = 0; x < width; ++x) {
      float hL = heightmap.getHeight(x - 1, z);
      float hR = heightmap.getHeight(x + 1, z);
      float hD = heightmap.getHeight(x, z - 1);
      float hU = heightmap.getHeight(x, z + 1);

      float nx = hL - hR;
      float ny = 2.0f * config.gridSpacing;
      float nz = hD - hU;

      // Normalize
      float len = sqrt(nx * nx + ny * ny + nz * nz);
      if (len > 0.0001f) {
        nx /= len;
        ny /= len;
        nz /= len;
      } else {
        nx = 0.0f;
        ny = 1.0f;
        nz = 0.0f;
      }

      int idx = z * width + x;
      m_mesh.normals[idx * 3 + 0] = nx;
      m_mesh.normals[idx * 3 + 1] = ny;
      m_mesh.normals[idx * 3 + 2] = nz;
    }
  }

  // --- Populate Per-Vertex Colors (Biome) ---
  for (int z = 0; z < depth; ++z) {
    for (int x = 0; x < width; ++x) {
      int idx = z * width + x;
      float height = heightmap.getHeight(x, z);
      float normalY = m_mesh.normals[idx * 3 + 1];

      Color color =
          getBiomeColor(height, normalY, config.maxHeight, config.seaLevel);

      m_mesh.colors[idx * 4 + 0] = color.r;
      m_mesh.colors[idx * 4 + 1] = color.g;
      m_mesh.colors[idx * 4 + 2] = color.b;
      m_mesh.colors[idx * 4 + 3] = color.a;
    }
  }

  // --- Populate Index Buffer (Two triangles per grid cell) ---
  int indexOffset = 0;
  for (int z = 0; z < depth - 1; ++z) {
    for (int x = 0; x < width - 1; ++x) {
      unsigned short topLeft = static_cast<unsigned short>(z * width + x);
      unsigned short topRight = static_cast<unsigned short>(z * width + x + 1);
      unsigned short bottomLeft =
          static_cast<unsigned short>((z + 1) * width + x);
      unsigned short bottomRight =
          static_cast<unsigned short>((z + 1) * width + x + 1);

      // Triangle 1
      m_mesh.indices[indexOffset++] = topLeft;
      m_mesh.indices[indexOffset++] = bottomLeft;
      m_mesh.indices[indexOffset++] = topRight;

      // Triangle 2
      m_mesh.indices[indexOffset++] = topRight;
      m_mesh.indices[indexOffset++] = bottomLeft;
      m_mesh.indices[indexOffset++] = bottomRight;
    }
  }

  // --- Upload to GPU ---
  UploadMesh(&m_mesh, false);

  // --- Create Model from Mesh ---
  m_model = LoadModelFromMesh(m_mesh);

  // Enable vertex colors in the material.
  // Set MATERIAL_MAP_DIFFUSE color to WHITE so per-vertex colors show through.
  m_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  m_loaded = true;
}

void TerrainRenderer::draw(Vector3 position, bool wireframe) const {
  if (!m_loaded)
    return;

  if (wireframe) {
    DrawModelWires(m_model, position, 1.0f, DARKGRAY);
  } else {
    DrawModel(m_model, position, 1.0f, WHITE);
  }
}

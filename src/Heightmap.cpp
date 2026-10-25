#include "Heightmap.h"
#include "FastNoiseLite.h"
#include "TerrainConfig.h"

#include <algorithm>
#include <cmath>

using namespace std;

void Heightmap::generate(const TerrainConfig &config) {
  m_width = config.width;
  m_depth = config.depth;
  m_data.resize(static_cast<size_t>(m_width) * m_depth);

  // configure nooise
  FastNoiseLite noise;
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetSeed(config.seed);
  noise.SetFrequency(config.baseFrequency);

  // configure fractal params
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalOctaves(config.octaves);
  noise.SetFractalLacunarity(config.lacunarity);
  noise.SetFractalGain(config.persistence);

  // This is da sample noise
  for (int z = 0; z < m_depth; ++z) {
    for (int x = 0; x < m_width; ++x) {

      float rawNoise =
          noise.GetNoise(static_cast<float>(x), static_cast<float>(z));

      float normalized = (rawNoise + 1.0f) * 0.5f;
      normalized = clamp(normalized, 0.0f, 1.0f);

      float shaped =
          pow(normalized, config.elevationPower) * config.maxHeight;

      m_data[static_cast<size_t>(z) * m_width + x] = shaped;
    }
  }
}

float Heightmap::getHeight(int x, int z) const {
  if (x < 0 || x >= m_width || z < 0 || z >= m_depth) {
    return 0.0f;
  }
  return m_data[static_cast<size_t>(z) * m_width + x];
}

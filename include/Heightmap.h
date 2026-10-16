#pragma once

#include "TerrainConfig.h"
#include <vector>

/// Manages contiguous memory storage and elevation synthesis.
/// Internal buffer: std::vector<float> of size width * depth in row-major order.
/// Index(x, z) = z * width + x
class Heightmap {
public:
    Heightmap() = default;

    /// Generate the heightmap from the given terrain configuration using fBm noise.
    void generate(const TerrainConfig& config);

    /// Get the height value at grid coordinates (x, z).
    float getHeight(int x, int z) const;

    /// Get grid width.
    int getWidth() const { return m_width; }

    /// Get grid depth.
    int getDepth() const { return m_depth; }

private:
    int m_width = 0;
    int m_depth = 0;
    std::vector<float> m_data;
};

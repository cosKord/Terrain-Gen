#pragma once

#include "TerrainConfig.h"
#include <vector>

class Heightmap {
public:
    Heightmap()=default;

    void generate(const TerrainConfig& config);
    float getHeight(int x, int z) const;
    int getWidth() const { return m_width; }
    int getDepth() const { return m_depth; }

private:
    int m_width = 0;
    int m_depth = 0;
    std::vector<float> m_data;
};

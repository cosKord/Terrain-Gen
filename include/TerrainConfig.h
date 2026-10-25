#pragma once

#include <cstdint>

struct TerrainConfig {
    int width = 256;
    int depth = 256;
    float gridSpacing = 1.0f;
    //thisthe seed
    int seed = 53;
    float baseFrequency = 0.01f;
    int   octaves       = 6;
    float lacunarity    = 2.0f;
    float persistence   = 0.5f;
    float elevationPower = 1.5f;
    float maxHeight = 50.0f;
    float seaLevel = 10.0f;
};

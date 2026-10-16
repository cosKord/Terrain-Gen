#pragma once

#include <cstdint>

/// Plain data container managing procedural generation parameters.
struct TerrainConfig {
    // Grid vertex resolution along X and Z axes
    int width = 256;
    int depth = 256;

    // World-space distance between adjacent vertices
    float gridSpacing = 1.0f;

    // Integer seed for deterministic generation
    int seed = 42;

    // fBm noise tuning parameters
    float baseFrequency = 0.01f;
    int   octaves       = 6;
    float lacunarity    = 2.0f;
    float persistence   = 0.5f;

    // Exponent applied to normalized height h^p
    // Low values flatten plains, high values sharpen peaks
    float elevationPower = 1.5f;

    // Vertical world-space amplitude
    float maxHeight = 50.0f;

    // Height threshold differentiating ocean from landmass
    float seaLevel = 10.0f;
};

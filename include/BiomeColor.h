#pragma once

#include "raylib.h"

/// Pure mathematical color mapping functions.
/// Evaluates normalized height and surface normal vertical component (Ny)
/// to return an 8-bit RGBA Color.
///
/// Biome zones:
///   - Deep/Shallow Water:  Y <= seaLevel (Blues)
///   - Sand / Coastline:    Just above sea level
///   - Lush Plains:         Medium elevations with flat slopes (Ny ~ 1.0)
///   - Steep Cliffs / Rock: Steep inclines (Ny < 0.65)
///   - Snow / High Alt:     High elevation peaks (Y > 0.85 * maxHeight)

/// Compute a biome color based on height, normal Y component, and terrain config values.
/// @param height     The world-space height of the vertex.
/// @param normalY    The Y component of the surface normal (1.0 = flat, 0.0 = vertical).
/// @param maxHeight  Maximum terrain height for normalization.
/// @param seaLevel   Sea level threshold.
/// @return           An RGBA Color value for the vertex.
Color getBiomeColor(float height, float normalY, float maxHeight, float seaLevel);

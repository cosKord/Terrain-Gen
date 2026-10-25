#include "BiomeColor.h"

#include <algorithm>

using namespace std;

static float smoothstep(float edge0, float edge1, float x) {
  float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  return t * t * (3.0f - 2.0f * t);
}

static Color lerpColor(Color a, Color b, float t) {
  return Color{
      static_cast<unsigned char>(a.r + static_cast<int>((b.r - a.r) * t)),
      static_cast<unsigned char>(a.g + static_cast<int>((b.g - a.g) * t)),
      static_cast<unsigned char>(a.b + static_cast<int>((b.b - a.b) * t)),
      255};
}

Color getBiomeColor(float height, float normalY, float maxHeight, float seaLevel) {

  constexpr Color deepWater = {30, 60, 150, 255};
  constexpr Color shallowWater = {50, 120, 200, 255};
  constexpr Color sand = {210, 200, 140, 255};
  constexpr Color grass = {50, 160, 50, 255};
  constexpr Color darkGrass = {30, 110, 30, 255};
  constexpr Color rock = {130, 120, 110, 255};
  constexpr Color snow = {240, 240, 250, 255};

  if (height < seaLevel * 0.6f) {
    return deepWater;
  }

  if (height < seaLevel) {
    float t = smoothstep(seaLevel * 0.6f, seaLevel, height);
    return lerpColor(deepWater, shallowWater, t);
  }

  if (normalY < 0.65f) {
    return rock;
  }

  float sandEnd = seaLevel + (maxHeight - seaLevel) * 0.05f;
  if (height < sandEnd) {
    float t = smoothstep(seaLevel, sandEnd, height);
    return lerpColor(shallowWater, sand, t);
  }

  float snowStart = maxHeight * 0.85f;
  if (height > snowStart) {
    float t = smoothstep(snowStart, maxHeight, height);
    return lerpColor(rock, snow, t);
  }

  float grassEnd = maxHeight * 0.5f;
  if (height < grassEnd) {
    float t = smoothstep(sandEnd, grassEnd, height);
    return lerpColor(sand, grass, t);
  }

  {
    float t = smoothstep(grassEnd, snowStart, height);
    return lerpColor(darkGrass, rock, t);
  }
}

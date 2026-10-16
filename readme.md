# Terrain-Gen

A procedural 3D terrain generator in C++20. It builds a heightmap from multi-octave noise (fBm), turns it into a triangle mesh, and colors it by height and slope.

## Features

- Deterministic, seed-based terrain using fractional Brownian motion (FastNoiseLite)
- Adjustable elevation curve for flat plains and sharper peaks
- Biome coloring (Just in beta for now): water, sand, grassland, rock cliffs, snow
- Free-fly camera to explore

## Built With

- C++20
- [Raylib 5.5](https://github.com/raysan5/raylib) (fetched automatically by CMake)
- [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) (header-only, in `vendor/`)

## Build & Run

Requires CMake 3.20+ and a C++20 compiler (GCC 11+, Clang 13+, or MSVC 19.29+).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/terrain_gen            # Windows: build\Release\terrain_gen.exe
```

## Project Structure

```
include/   TerrainConfig, Heightmap, BiomeColor, TerrainRenderer headers
src/       main loop, noise sampling, biome colors, mesh building
vendor/    FastNoiseLite.h
```

## How It Works

1. Sample fBm noise on a 2D grid and reshape it with `h^elevationPower`.
2. Build an indexed mesh with two triangles per grid cell.
3. Compute normals, then pick colors from height and slope.
4. Upload the mesh to the GPU and render.
```
Author notes: This is basically a for-fun project, to better my understanding of C++ (in scaffold phase for now). Any suggestions are welcome!

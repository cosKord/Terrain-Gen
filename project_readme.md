s# Procedural 3D Terrain Generator (C++20 / Raylib)

A modular, real-time procedural terrain generation engine implemented in modern C++20 and Raylib. The system synthesizes coherent multi-octave noise (Fractional Brownian Motion) across a regular 2D grid, constructs dynamic indexed triangle meshes, computes surface normal vectors via finite differences, and performs height/slope-based biome classification.

---

## 1. Directory & File Structure

```text
procedural-terrain/
├── CMakeLists.txt                  # Root build configuration (FetchContent for Raylib)
├── README.md                       # Setup, architectural overview, and run instructions
├── vendor/
│   └── FastNoiseLite.h             # Header-only coherent noise library (Auburn)
├── include/
│   ├── TerrainConfig.h             # Parameter struct (seed, octaves, persistence, scaling)
│   ├── Heightmap.h                 # Discrete 2D grid storage and algorithmic fBm generation
│   ├── BiomeColor.h                # Color interpolation functions (elevation & slope to RGB)
│   └── TerrainRenderer.h           # Geometry builder (converts Heightmap into Raylib Mesh)
└── src/
    ├── main.cpp                    # Application loop, camera controls, and input polling
    ├── Heightmap.cpp               # Noise sampling, domain warping, and exponential shaping
    ├── BiomeColor.cpp              # Biome gradient thresholds and smoothstep transitions
    └── TerrainRenderer.cpp         # Index buffer wiring, normal vector cross products, Raylib upload
```

---

## 2. Prerequisites & Toolchain

Ensure the following tools are installed and present in your system `PATH`:

* **C++ Compiler:** Supporting **C++20** standard (`MSVC v19.29+` / `GCC 11+` / `Clang 13+`).
* **Build System:** **CMake 3.20+**.
* **Make/Generator:** `Ninja` (recommended for rapid builds) or system default (`Make` / `MSVC MSBuild`).
* **Git:** Required by CMake `FetchContent` to clone Raylib automatically.
* **External Header:** Download `FastNoiseLite.h` and place it inside `vendor/`:
  * Upstream repository: [Auburn/FastNoiseLite](https://github.com/Auburn/FastNoiseLite)

---

## 3. CMake Configuration (`CMakeLists.txt`)

Save the following configuration as `CMakeLists.txt` in the project root. It handles automated downloading, compilation, and linking of **Raylib 5.5**:

```cmake
cmake_minimum_required(VERSION 3.20)
project(ProceduralTerrain LANGUAGES CXX)

# Enforce Modern C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependency: Raylib (Automatic FetchContent)
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# Header Search Paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# Project Source Files
set(SOURCES
    src/main.cpp
    src/Heightmap.cpp
    src/BiomeColor.cpp
    src/TerrainRenderer.cpp
)

# Executable Target
add_executable(terrain_gen ${SOURCES})

# Link Libraries
target_link_libraries(terrain_gen PRIVATE raylib)

# Compiler Warning Flags
if (MSVC)
    target_compile_options(terrain_gen PRIVATE /W4 /permissive-)
else()
    target_compile_options(terrain_gen PRIVATE -Wall -Wextra -Wpedantic)
endif()
```

---

## 4. Module Architecture & Class Responsibilities

### `TerrainConfig.h`
A plain data container managing procedural generation parameters:
* `width`, `depth`: Grid vertex resolution along the $X$ and $Z$ axes (e.g., $256 \times 256$).
* `gridSpacing`: World-space distance between adjacent vertices.
* `seed`: Integer seed for deterministic generation.
* `baseFrequency`, `octaves`, `lacunarity`, `persistence`: fBm noise tuning parameters.
* `elevationPower`: Exponent $p$ applied to normalized height $h^p$ for low-elevation flattening (plains) and peak sharpening.
* `maxHeight`: Vertical world-space amplitude.
* `seaLevel`: Height threshold differentiating ocean surfaces from landmass.

### `Heightmap.h` / `Heightmap.cpp`
Manages contiguous memory storage and elevation synthesis:
* Internal buffer: `std::vector<float>` of size `width * depth` laid out in row-major order:
  $$\text{Index}(x, z) = z \cdot \text{width} + x$$
* Implements multi-octave sampling using `FastNoiseLite`.
* Applies redistribution math: transforms raw noise $n \in [-1, 1] \rightarrow h \in [0, 1]$, then applies $h_{\text{shaped}} = h^{\text{elevationPower}} \times \text{maxHeight}$.

### `BiomeColor.h` / `BiomeColor.cpp`
Pure mathematical color mapping functions:
* Evaluates normalized height and the surface normal's vertical component ($N_y$).
* Returns an 8-bit RGBA `Color` (`unsigned char r, g, b, a`):
  * **Deep/Shallow Water:** $Y \le \text{seaLevel}$ (Blues)
  * **Sand / Coastline:** Just above sea level
  * **Lush Plains / Lowlands:** Medium elevations with flat slopes ($N_y \approx 1.0$)
  * **Steep Cliffs / Rock:** Steep inclines ($N_y < 0.65$) regardless of mid-tier height
  * **Snow / High Altitude:** High elevation peaks ($Y > 0.85 \times \text{maxHeight}$)

### `TerrainRenderer.h` / `TerrainRenderer.cpp`
Converts abstract mathematical grid data into GPU-ready buffers:
* Allocates and populates Raylib's `Mesh` structure arrays:
  * `mesh.vertices`: $3 \times V$ floats representing $(X, Y, Z)$.
  * `mesh.normals`: $3 \times V$ floats computed via cross product of adjacent grid points:
    $$\vec{N} = \text{normalize}\left(\langle h(x-1, z) - h(x+1, z), \ 2 \cdot \text{gridSpacing}, \ h(x, z-1) - h(x, z+1) \rangle\right)$$
  * `mesh.colors`: $4 \times V$ unsigned chars for per-vertex biome tinting.
  * `mesh.indices`: $6 \times (W-1) \times (D-1)$ unsigned shorts/ints assembling two triangles per grid cell.
* Calls `UploadMesh(&mesh, false)` to bind VAO/VBOs to the GPU.
* Encapsulates memory management with an RAII destructor calling `UnloadMesh(mesh)`.

### `main.cpp`
Entry point and event loop:
* Initializes the window context (`InitWindow`) and sets target refresh rate (`SetTargetFPS(60)`).
* Sets up a `Camera3D` in `CAMERA_FREE` mode (mouse look and WASD translation).
* Polls keyboard inputs for dynamic runtime regeneration:
  * `KEY_R`: Generates a randomized seed and rebuilds buffers.
  * `KEY_UP` / `KEY_DOWN`: Increments/decrements `elevationPower`.
  * `KEY_SPACE`: Toggles wireframe view vs. solid shaded view.

---

## 5. Build & Execution Workflow

### Quick Setup Commands

Run the following commands from your terminal inside the project directory:

```bash
# 1. Create project directories
mkdir -p include src vendor assets/shaders

# 2. Download FastNoiseLite single-header library into vendor/
# (Place FastNoiseLite.h inside the vendor/ directory)

# 3. Generate native build configuration
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 4. Compile the project
cmake --build build --config Release

# 5. Execute binary
# On Linux/macOS:
./build/terrain_gen

# On Windows:
./build/Release/terrain_gen.exe
```

---

## 6. Implementation Milestones

1. **Step 1:** Establish compilation with CMake; render an empty scene with an active Raylib 3D free-fly camera and coordinate reference grid (`DrawGrid`).
2. **Step 2:** Construct a flat $N \times N$ planar mesh in `TerrainRenderer.cpp` and verify correct triangle winding order without visual artifacts.
3. **Step 3:** Integrate `Heightmap.cpp` with `FastNoiseLite`, modulating the $Y$ vertex component using fBm octaves and non-linear power curves.
4. **Step 4:** Calculate smooth surface normals and populate `mesh.colors` based on height and slope thresholds.
5. **Step 5:** Bind runtime key controls to trigger mesh reconstruction without memory leaks.
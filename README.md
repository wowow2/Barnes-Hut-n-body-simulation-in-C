# 3-Galaxy Collision N-Body Simulation in C

This is a classic gravitational N-body simulation implemented in C. It uses the Barnes-Hut algorithm to achieve an **O(n log n)** time complexity. This specific version is configured to run a single scenario: the collision and merger of three distinct disc galaxies.

---

## Overview

The program initializes three stable, orbiting disc galaxies, places them in a triangular formation, and gives them an initial velocity aimed towards the center. The simulation then runs in a non-interactive "batch mode," calculating the gravitational interactions over thousands of time steps as the galaxies collide, distort, and eventually merge into a single, larger cluster.

The core of the project is a modular C physics engine:
- **`vector`**: Handles all 2D vector mathematics.
- **`body`**: Represents a single physical particle (a star or black hole).
- **`quadtree`**: Manages the spatial partitioning of bodies.
- **`barnes_hut`**: Implements the efficient O(n log n) force calculation algorithm.
- **`simulation`**: Contains the main time-step loop.

---

## Barnes-Hut Algorithm 

The naive N-body algorithm computes the gravitational force between every pair of bodies, resulting in **O($n^2$)** operations. This quickly becomes infeasible for large systems. The **Barnes-Hut algorithm** addresses this by organizing bodies into a hierarchical quadtree (2D) or octree (3D) and approximating distant clusters as single masses at their centers of mass. This reduces the computational complexity to **O(n log n)** while maintaining accuracy for nearby interactions. The algorithm proceeds in three main steps: building the quadtree, recursively computing forces using approximations for distant nodes, and updating particle positions and velocities with time integration. This approach enables efficient simulation of large-scale systems.

### Quadtree Construction and Insertion (`quadtree.c` / `quadtree.h`)

The quadtree is a hierarchical spatial partitioning structure used to accelerate force calculations in the Barnes-Hut algorithm. Each node represents a square region of space and may either be:
- **A leaf node**, containing a single body (particle).
- **An internal node**, containing four child quadrants (NW, NE, SW, SE) and a combined center of mass of all bodies in that region.

#### Key Features

- **Node creation**: `quadtree_create()` initializes a node with a center, size, and empty children.
- **Recursive insertion**: `quadtree_insert_recursive()` places a body into the correct quadrant:
    - If the node is empty, the body is stored directly.
    - If the node is already occupied, the node subdivides into four children (`quadtree_subdivide()`), and both the existing and new bodies are re-inserted recursively.
    - Subdivision continues up to a `MAX_DEPTH` to prevent infinite recursion.
- **Quadrant determination**: `get_quadrant()` calculates the appropriate child quadrant for a body based on its position relative to the node’s center.
- **Mass distribution**: `quadtree_compute_mass_distribution()` recursively calculates the total mass and center of mass for each node, which is used for approximate force computations in the Barnes-Hut algorithm.
- **Memory management**: `quadtree_free()` recursively frees all nodes in the tree.

### Recursive Force Computation (`barnes_hut.c` / `barnes_hut.h`)

After the quadtree is built, the simulation calculates gravitational forces on each body efficiently using the hierarchical structure:
- **Traversal**: For each body, the algorithm starts at the root of the quadtree and recursively traverses nodes.
- **Distance criterion**: At each node, the ratio of the node’s size to the distance from the body (s/d) is compared to a threshold θ:
  - If `s/d < θ`, the node is sufficiently far and is treated as a single mass at its center of mass.
  - Otherwise, the algorithm recurses into the node’s children for more accurate calculations.
- **Force accumulation**: Forces are computed according to Newton’s law of gravitation and summed for each body.
- **Function reference**: `compute_force(Body *b, QuadtreeNode *node, double theta)` performs the recursive calculation and updates the acceleration of the body.

This approach avoids computing every pairwise interaction, reducing the complexity from O(n²) to approximately O(n log n), while maintaining accuracy for nearby particles.

## Body Structure (`body.h` / `body.c`)

Each `Body` represents a single particle in the simulation and stores the following attributes. A 2D vector datatype is defined in `vector.h` / `vector.c` along with basic vector operations.

- **position** (`Vec2`): Current coordinates of the body.
- **velocity** (`Vec2`): Current velocity vector.
- **acceleration** (`Vec2`): Current acceleration vector, updated each timestep from forces.
- **mass** (`double`): Mass of the body, used to calculate gravitational acceleration.

These fields are used by `applyForce()`, `update_body()`, and `reset_acceleration()` to manage dynamics during the simulation.

## Time Integration (`simulation.c` / `simulation.h`)

Once accelerations are computed, the simulation updates each body’s velocity and position to advance the system in time:

- **Semi-implicit Euler method**: Velocities are first updated using the current acceleration, then positions are updated using the newly computed velocities:
```c
v_new = v_old + a * dt;
p_new = p_old + v_new * dt;
```
- **Velocity update**: Each body’s velocity is incremented by its current acceleration multiplied by the timestep `dt`.
- **Position update**: Each body’s position is incremented by its updated velocity multiplied by `dt`.
- **Simulation loop**: This process repeats for the desired number of timesteps:
  1. Rebuild the quadtree with updated positions.
  2. Recompute forces using the Barnes-Hut algorithm.
  3. Update velocities and positions.

## Visualization
The simulation outputs a CSV file (`output_3_galaxy_collision.csv`) containing the positions of all bodies at each timestep. A Python script (`visualize.py`) uses this data to generate frames and create a video:

- The script uses `pandas` to load the CSV and `matplotlib` to render stars and black holes.  
- Frames are plotted efficiently, with black holes highlighted in cyan and stars in white.  
- Multi-processing (`multiprocessing.Pool`) speeds up frame rendering across available CPU cores.  
- Position limits are dynamically determined using quantiles to focus the view on the main galaxy structures.  
- Each frame is saved as a PNG in the `frames/` directory, ready for compilation into a video using FFmpeg.

## Compilation and Usage
```bash
# --- Compilation ---
# Compile all C source files into an executable named 'nbody_3_galaxy'
# All source and header files are expected to be in a 'src/' directory.
gcc src/*.c -o nbody_3_galaxy -Isrc -O2 -lm -Wall -Wextra -std=c11

# --- Usage ---
# Run the simulation
./nbody_3_galaxy

# The program will print its progress to the terminal and, when complete,
# will generate a data file named output_3_galaxy_collision.csv.

# --- Visualization ---
# 1) Install Python dependencies
pip install pandas matplotlib numpy tqdm

# 2) Render frames (creates PNGs in frames/ directory)
python visualize.py

# 3) Create video with FFmpeg
ffmpeg -framerate 30 -i frames/frame_%05d.png -c:v libx264 -pix_fmt yuv420p -crf 18 output_collision.mp4

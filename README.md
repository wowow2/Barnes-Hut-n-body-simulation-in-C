# 3-Galaxy Collision N-Body Simulation in C

This is a classic gravitational N-body simulation implemented in C. It uses the Barnes-Hut algorithm to achieve an **O(n log n)** time complexity. This specific version is configured to run a single, spectacular scenario: the collision and merger of three distinct disc galaxies.

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

## Compilation, Usage & Visualization

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
python render_frames.py

# 3) Create video with FFmpeg
ffmpeg -framerate 30 -i frames/frame_%05d.png -c:v libx264 -pix_fmt yuv420p -crf 18 output_collision.mp4

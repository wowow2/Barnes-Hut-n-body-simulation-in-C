import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import os, sys
from multiprocessing import Pool, cpu_count
from tqdm import tqdm
import numpy as np

INPUT_FILE = "output_3_galaxy_collision.csv"
OUTPUT_DIR = "frames"

STAR_COLOR = 'white'
STAR_SIZE = 0.5   # smaller marker for speed
BH_COLOR = 'cyan'
BH_SIZE = 12.0

VIEW_PADDING = 1.4
QUANTILE_LOW = 0.05
QUANTILE_HIGH = 0.95

def render_frame(args):
    step, stars, bhs, limits, output_dir = args
    (xlim, ylim) = limits

    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_facecolor('black')
    fig.set_facecolor('black') 

    ax.set_xlim(*xlim)
    ax.set_ylim(*ylim)
    ax.axis("off")

    # Faster plotting using `plot` with markers ','
    if stars.size > 0:
        ax.plot(stars[:,0], stars[:,1], ',', color=STAR_COLOR, markersize=STAR_SIZE)
    if bhs.size > 0:
        ax.scatter(bhs[:,0], bhs[:,1], s=BH_SIZE, color=BH_COLOR, edgecolors='white', linewidths=0.3)

    filename = os.path.join(output_dir, f"frame_{step:05d}.png")
    plt.savefig(filename, dpi=100, bbox_inches="tight", pad_inches=0)
    plt.close(fig)
    return step

if __name__ == "__main__":
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    print(f"Loading data from {INPUT_FILE}...")
    df = pd.read_csv(INPUT_FILE, header=None, names=['step','id','x','y'],
                     usecols=[0,1,2,3],
                     dtype={'step':int,'id':int,'x':float,'y':float},
                     na_values=['inf','-inf'])
    df.dropna(inplace=True)

    total_bodies = df['id'].max() + 1
    n1 = total_bodies // 3
    n2 = total_bodies // 3
    black_hole_ids = {0, n1, n1+n2}
    print(f"Detected {total_bodies} bodies. Black hole IDs: {black_hole_ids}")

    tasks = []
    for step, group in df.groupby('step'):
        stars_mask = ~group['id'].isin(black_hole_ids)
        stars = group.loc[stars_mask, ['x','y']].to_numpy(np.float32)
        bhs   = group.loc[~stars_mask, ['x','y']].to_numpy(np.float32)

        if stars.size > 0:
            x_min, x_max = np.quantile(stars[:,0], [QUANTILE_LOW, QUANTILE_HIGH])
            y_min, y_max = np.quantile(stars[:,1], [QUANTILE_LOW, QUANTILE_HIGH])
            cx, cy = (x_min+x_max)/2, (y_min+y_max)/2
            size = max(x_max-x_min, y_max-y_min) * VIEW_PADDING
            if size < 1: size = 100
            limits = [(cx-size/2, cx+size/2), (cy-size/2, cy+size/2)]
        else:
            limits = [(-1,1), (-1,1)]

        tasks.append((step, stars, bhs, limits, OUTPUT_DIR))

    print(f"Prepared {len(tasks)} frames for rendering...")

    num_processes = max(1, cpu_count()-1)
    with Pool(num_processes) as pool:
        for _ in tqdm(pool.imap_unordered(render_frame, tasks), total=len(tasks)):
            pass

    print("All frames rendered!")

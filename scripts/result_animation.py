import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import imageio

# Booth function
# RANGE_X = [-10, 10]
# RANGE_Y = [-10, 10]
# GROUND_TRUTH = "f(1, 3)=0"

# Eggholder function
RANGE_X = [-512, 512]
RANGE_Y = [-512, 512]
GROUND_TRUTH = "f(512, 404.2319)=-959.6407"

def booth_func(x, y):
    return (x + 2 * y - 7)**2 + (2 * x + y - 5)**2

def eggholder_func(x, y):
    return -(y + 47) * np.sin(np.sqrt(abs(x/2 + (y + 47)))) - x * np.sin(np.sqrt(abs(x - (y + 47))))

def main():

    x = np.linspace(*RANGE_X, 400)
    y = np.linspace(*RANGE_Y, 400)
    X, Y = np.meshgrid(x, y)
    Z = eggholder_func(X, Y)

    df = pd.read_csv("../results/history.csv")
    iterations = df["iteration"].unique()

    # list for save the temporary images
    filenames = []

    for itr in iterations:
        subset = df[df["iteration"] == itr]
        mask = subset["fitness"] == subset["fitness"].min()
        best_solution = [subset[mask]["x0"].values[0], subset[mask]["x1"].values[0]]
        best_fitness = subset[mask]["fitness"].values[0]

        fig, ax = plt.subplots(figsize=(10, 6))
        contour = ax.contour(X, Y, Z, levels=30, cmap="viridis", linewidths=1, zorder=1)
        plt.scatter(subset["x0"], subset["x1"], c="red", s=15, zorder=2)
        plt.title(f"Iteration {itr}, Best: f({best_solution[0]:.4f}, {best_solution[1]:.4f})={best_fitness:.2f}, \nGround Truth: {GROUND_TRUTH}")
        plt.xlabel("x0")
        plt.ylabel("x1")
        plt.grid(True)
        ax.set_xlim([RANGE_X[0], RANGE_X[1]])
        ax.set_ylim([RANGE_Y[0], RANGE_Y[1]])

        # save temporary image
        filename = f"frame_{itr}.png"
        plt.savefig(filename)
        filenames.append(filename)

        plt.close()

    # create GIF
    with imageio.get_writer("agent_movement.gif", mode="I", duration=0.2, loop=1) as writer:
        for filename in filenames:
            image = imageio.imread(filename)
            writer.append_data(image)

    # delete temporary images
    for filename in filenames:
        os.remove(filename)

if __name__ == "__main__":
    main()
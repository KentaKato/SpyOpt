import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import imageio

RANGE_X = [-10, 10]
RANGE_Y = [-10, 10]
def main():
    def func(x, y):
        return (x + 2 * y - 7)**2 + (2 * x + y - 5)**2

    x = np.linspace(*RANGE_X, 400)
    y = np.linspace(*RANGE_Y, 400)
    X, Y = np.meshgrid(x, y)
    Z = func(X, Y)

    df = pd.read_csv("../results/history.csv")
    iterations = df["iteration"].unique()

    print(iterations)

    # list for save the temporary images
    filenames = []

    for itr in iterations:
        if (itr % 3 != 0) and (itr != iterations[-1]):
            continue

        subset = df[df["iteration"] == itr]
        mask = subset["fitness"] == subset["fitness"].min()

        fig, ax = plt.subplots(figsize=(5, 3))
        contour = ax.contour(X, Y, Z, levels=50, cmap="autumn")
        plt.scatter(subset["x0"], subset["x1"],
                    c=subset["fitness"], s=10, cmap="cool")
        plt.colorbar()
        plt.title(f"Iteration {itr}")
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
    with imageio.get_writer("agent_movement.gif", mode="I", duration=0.1) as writer:
        for filename in filenames:
            image = imageio.imread(filename)
            writer.append_data(image)

    # delete temporary images
    for filename in filenames:
        os.remove(filename)

if __name__ == "__main__":
    main()
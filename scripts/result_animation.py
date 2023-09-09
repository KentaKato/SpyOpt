from typing import List, Callable

import os
import pandas as pd
from pandas import DataFrame
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import imageio

def booth_func(x: float, y: float) -> float:
    return (x + 2 * y - 7)**2 + (2 * x + y - 5)**2

def eggholder_func(x: float, y: float) -> float:
    return -(y + 47) * np.sin(np.sqrt(abs(x/2 + (y + 47)))) - x * np.sin(np.sqrt(abs(x - (y + 47))))

def ackley_func(x:float, y:float) -> float:
    a = 20
    b = 0.2
    c = 2 * np.pi
    sum1 = x**2 + y**2
    sum2 = np.cos(c*x) + np.cos(c*y)
    return -a * np.exp(-b * np.sqrt(sum1 / 2)) - np.exp(sum2 / 2) + a + np.exp(1)

def draw_objective_function(objective_func: Callable[[float, float], float],
                            range_x: List[float],
                            range_y: List[float],
                            num_div: int=400) -> Callable[[matplotlib.axes.Axes], None]:
    x = np.linspace(*range_x, num_div)
    y = np.linspace(*range_y, num_div)
    X, Y = np.meshgrid(x, y)
    Z = objective_func(X, Y)
    def draw(ax: matplotlib.axes.Axes) -> None:
        ax.contour(X, Y, Z, levels=30, cmap="viridis", linewidths=1, zorder=1)
    return draw

def create_gif(filenames: List[str],
               loop_pause_duration: float=5.0,
               frame_duration: float=0.2) -> None:
    with imageio.get_writer("agent_movement.gif",
                            mode="I",
                            duration=frame_duration,
                            loop=0) as writer:
        for idx, filename in enumerate(filenames):
            image = imageio.imread(filename)
            if idx == len(filenames) - 1:  # 最後のフレームの場合
                writer.append_data(image)

                def append_loop_pause(writer, duration):
                    for _ in range(int(loop_pause_duration / 0.2)):
                        writer.append_data(image)
                append_loop_pause(writer, loop_pause_duration)
            else:
                writer.append_data(image)


def main():

    OBJECTIVE_FUNC_NAME = "Eggholder Function" # "Booth Function"/"Eggholder Function"/"Ackley Function"

    if OBJECTIVE_FUNC_NAME == "Booth Function":
        OBJECTIVE_FUNCTION = booth_func
        RANGE_X = [-10., 10.]
        RANGE_Y = [-10., 10.]
        GROUND_TRUTH = "f(1, 3)=0"
        GROUND_TRUTH_SOLUTION = [1., 3.]

    elif OBJECTIVE_FUNC_NAME == "Eggholder Function":
        OBJECTIVE_FUNCTION = eggholder_func
        RANGE_X = [-512., 512.]
        RANGE_Y = [-512., 512.]
        GROUND_TRUTH = "f(512, 404.2319)=-959.6407"
        GROUND_TRUTH_SOLUTION = [512, 404.2319]

    elif OBJECTIVE_FUNC_NAME == "Ackley Function":
        OBJECTIVE_FUNCTION = ackley_func
        RANGE_X = [-5., 5.]
        RANGE_Y = [-5., 5.]
        GROUND_TRUTH = "f(0, 0)=0"
        GROUND_TRUTH_SOLUTION = [0., 0.]

    else:
        raise ValueError("Invalid obejective_func_name")

    draw_objective: Callable[[matplotlib.axes.Axes], None] = draw_objective_function(OBJECTIVE_FUNCTION,
                                                                                     RANGE_X,
                                                                                     RANGE_Y)

    df = pd.read_csv("../results/history.csv")
    iterations = df["iteration"].unique()

    # list for save the temporary images
    filenames = []

    for itr in iterations:
        subset: DataFrame = df[df["iteration"] == itr]
        mask = subset["fitness"] == subset["fitness"].min()
        best_solution: List[float] = [subset[mask]["x0"].values[0], subset[mask]["x1"].values[0]]
        best_fitness: float = subset[mask]["fitness"].values[0]

        fig, ax = plt.subplots(figsize=(10, 6))
        draw_objective(ax)

        # plot agents
        plt.scatter(GROUND_TRUTH_SOLUTION[0], GROUND_TRUTH_SOLUTION[1],
                    c="lime", marker="*", s=200, zorder=2)
        plt.scatter(subset["x0"], subset["x1"], c="red", s=15, zorder=3)

        plt.title(f"{OBJECTIVE_FUNC_NAME}\nIteration {itr}, Best: f({best_solution[0]:.4f}, {best_solution[1]:.4f})={best_fitness:.4f}, \nGround Truth: {GROUND_TRUTH}")
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

    create_gif(filenames)

    # delete temporary images
    for filename in filenames:
        os.remove(filename)

if __name__ == "__main__":
    main()
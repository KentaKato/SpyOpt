from typing import List, Callable

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def process_data(num_epochs):
    all_data = []

    for epoch in range(num_epochs):
        filename = f"../results/multi_evaluation/best_solution_history_{epoch}.csv"
        df = pd.read_csv(filename)
        all_data.append(df["fitness"].values)

    all_data = np.vstack(all_data).T  # ここを変更
    medians = np.median(all_data, axis=1)
    p25 = np.percentile(all_data, 25, axis=1)
    p75 = np.percentile(all_data, 75, axis=1)

    return medians, p25, p75

def plot_data(num_epochs, num_iterations, title):
    medians, p25, p75 = process_data(num_epochs)

    plt.figure(figsize=(10, 6))
    plt.plot(range(num_iterations), medians, label="Median", color="blue")
    plt.fill_between(range(num_iterations), p25, p75, color="blue", alpha=0.2, label="25th-75th Percentile")
    plt.xlabel("Iteration")
    plt.ylabel("Fitness")
    plt.legend()
    plt.title(title)
    plt.grid(True)
    plt.tight_layout()
    plt.show()

def main():
    num_epochs = 300
    num_iteraions = 50
    num_agents = 100
    title = f"Fitness Statistics over {num_epochs} epochs, {num_iteraions} iterations, {num_agents} Agents for Eggholder Function"
    plot_data(num_epochs, num_iteraions, title)


if __name__ == "__main__":
    main()
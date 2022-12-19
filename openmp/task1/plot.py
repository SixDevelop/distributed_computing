"""Plot OpenMP test results for different threads num."""
from typing import Tuple

import matplotlib.pyplot as plt


def read_coords_from_file(filepath: str) -> Tuple[int, float]:
    X, Y = [], []

    for line in open(filepath):
        values = line.split()
        X.append(int(values[0]))
        Y.append(float(values[1]))
    return X, Y


def plot_thread_test_result() -> None:
    x, y = read_coords_from_file('thread_test.txt')

    plt.axhline(y=3572, color='orange',  linestyle = '-', label='Sequential')

    plt.plot(x, y, label = "OMP")
    plt.title("Время выполнения, N = 10000")
    plt.scatter(x, y, color='blue', marker='o')
    plt.xlabel("Число потоков")
    plt.legend()
    plt.savefig('thread_test.png')

if __name__ == '__main__':
    # plot_test_result()
    plot_thread_test_result()
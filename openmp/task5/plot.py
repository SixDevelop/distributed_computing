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



def plot_thread_test_result(filename: str) -> None:
    x, y = read_coords_from_file(filename)

    plt.axhline(y=155, color='orange',  linestyle = '-', label='Sequential')

    plt.plot(x, y, label = "OMP")
    plt.title("Время выполнения, милисекунды, N = 100")
    plt.scatter(x, y, color='blue', marker='o')
    plt.xlabel("Число потоков")
    plt.legend()
    plt.savefig('thread_test.png')

if __name__ == '__main__':
    plot_thread_test_result('thread_test.txt')
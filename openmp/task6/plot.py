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


def plot_test_result() -> None:
    x_reduction, y_reduction = read_coords_from_file('reduction.txt')
    x_atomic, y_atomic = read_coords_from_file('atomic.txt')
    x_critical, y_critical = read_coords_from_file('critical.txt')
    x_lock, y_lock = read_coords_from_file('lock.txt')


    plt.plot(x_reduction, y_reduction, label="reduction")
    plt.plot(x_atomic, y_atomic, label="atomic")
    plt.plot(x_critical, y_critical, label="critical")
    plt.plot(x_lock, y_lock, label="lock")
    plt.legend()
    plt.title("Время выполнения, микросекунды")
    plt.xlabel("Число потоков")
    plt.legend()
    plt.savefig('thread_test.png')

    plt.savefig('thread_test.png')


if __name__ == '__main__':
    plot_test_result()
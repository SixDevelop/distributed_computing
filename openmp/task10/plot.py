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
    x1, y1 = read_coords_from_file('thread_test.txt')
    x2, y2 = read_coords_from_file('nested_parallelism_thread_test.txt')

    plt.axhline(y=305, color='green',  linestyle = '-', label='Sequential')

    plt.plot(x1, y1, label = "Parallel")
    plt.plot(x2, y2, label = "Nested Parallelism")
    plt.title("Время выполнения, милисекунды, N = 10000")
    plt.scatter(x1, y1, color='blue', marker='o')
    plt.scatter(x2, y2, color='brown', marker='o')
    plt.xlabel("Число потоков")
    plt.legend()
    plt.savefig('thread_test.png')

if __name__ == '__main__':
    # plot_test_result()
    plot_thread_test_result()
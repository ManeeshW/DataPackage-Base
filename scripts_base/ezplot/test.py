import ezplot as ezp
import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":

    x = np.arange(1, 10)
    y = x

    fig1 = ezp.SubPlot(1, 3)
    fig1.suptitle('Test1')

    fig1.subplot(1)
    fig1.plot(x, y)
    fig1.plot(x, -y)

    fig1.subplot(2)
    fig1.plot(-x, y)

    plt.show()

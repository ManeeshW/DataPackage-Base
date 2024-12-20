from __future__ import print_function
from mpl_toolkits.mplot3d import Axes3D

import copy
import itertools
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pdb


def plot(*args, **kwargs):

    plot_kwargs = {}
    mode = None
    title = ''
    legend_on = False
    fig_size = (0, 0)

    for key, value in kwargs.items():
        if key == 'mode':
            mode = value
        elif key == 'title':
            title = value
        elif key == 'label':
            legend_on = True
            legends = value
        elif key == 'legend':
            legend_on = True
            legends = value
        elif key == 'figsize':
            fig_size = value
        else:
            plot_kwargs[key] = value

    if mode == None:
        x, y, num_subplots = select_mode(*args)
        m, n = num_subplots

        if fig_size[0] == 0:
            fig = plt.figure()
        else:
            fig = plt.figure(figsize=fig_size)

        for i, j in list(itertools.product(range(len(y)), range(m*n))):
            # pdb.set_trace()
            try:
                ax = plt.subplot(m, n, j+1)
            except TypeError:
                pass
            
            if legend_on:
                plt.plot(x[i][j, :], y[i][j, :], label=legends[i], \
                    **plot_kwargs)
            else:
                plt.plot(x[i][j, :], y[i][j, :], **plot_kwargs)
        
        if legend_on:
            plt.legend()

    elif mode == '3d':
        fig = plt.figure()
        ax = fig.gca(projection='3d')

        ax.plot(args[0], args[1], args[2])
        ax.axis('equal')


    plt.suptitle(title)

    return


def select_mode(*args):

    num_args = len(args)
    all_x = []
    all_y = []
    len_x = 0

    if num_args % 2 and not num_args == 1:
        raise ValueError('data must be in the format (x1, y1, x2, y2, ...)')

    mid  = int(num_args/2)
    for i in range(mid):
        x_m, x_n = get_size(args[i])
        y_m, y_n = get_size(args[mid + i])
        if not x_n == y_n:
            raise ValueError('each (x, y) must have same lengths\n'
                             'for pair {}, the length of x is {}, but the '
                             'length of y is {}'.format(i % 2, len(args[i]),
                             len(args[mid + i])))
    flag_first_run = True
    for i, arg in enumerate(args):
        if i % 2:
            m, n = get_size(arg)
            if m > n:
                m, n = n, m
                all_y.append(arg.T)
            else:
                all_y.append(arg)

            if flag_first_run:
                flag_first_run = False
                num_data_series = m

        else:
            m, n = get_size(arg)
            if flag_first_run:
                num_data_series = m
            num_data_points = n

            if m > n:
                m, n = n, m
                all_x.append(arg.T)
            else:
                all_x.append(arg)

    x, y = clean_xy(all_x, all_y)
    n_rows, n_cols = get_subplot_dims(m)

    return x, y, (n_rows, n_cols)


def clean_xy(all_x, all_y):

    num_series = len(all_x)
    x = [[] for i in range(num_series)]
    for i, data_series in enumerate(all_x):
        num_axes, num_data_points = get_size(data_series)
        x[i] = np.zeros((num_axes, num_data_points))
        for ax in range(num_axes):
            if num_axes == 1:
                x[i][ax, :] = data_series
            else:
                x[i][ax, :] = data_series[ax, :]

    if len(all_y) == 0:
        y = copy.deepcopy(x)
        num_axes, num_data_points = get_size(data_series)
        x[i] = np.zeros((num_axes, num_data_points))
        for i, data_series in enumerate(y):
            num_axes, num_data_points = get_size(data_series)
            x[i] = np.zeros((num_axes, num_data_points))
            for ax in range(num_axes):
                x[i][ax, :] = np.arange(0, len(data_series[0]))
    else:
        y = [[] for num_series in range(num_series)]
        for i, data_series in enumerate(all_y):
            num_axes, num_data_points = get_size(data_series)
            y[i] = np.zeros((num_axes, num_data_points))
            for ax in range(num_axes):
                if num_axes == 1:
                    y[i][ax, :] = data_series
                else:
                    y[i][ax, :] = data_series[ax, :]

        # this part is needed for matching x values with multiple data series
        x = [[] for i in range(num_series)]
        for i, data_series in enumerate(all_x):
            temp, num_data_points = get_size(data_series)
            x[i] = np.zeros((num_axes, num_data_points))
            for ax in range(num_axes):
                x[i][ax, :] = data_series

    return x, y


def get_subplot_dims(m):

    if m==4:
        n_rows = 2
        n_cols = 2
    elif m==6:
        n_rows = 3
        n_cols = 2
    elif m==9:
        n_rows = 3
        n_cols = 3
    else:
        n_rows = m
        n_cols = 1

    return n_rows, n_cols


def get_size(arr):
    try:
        m, n = np.shape(arr)
    except ValueError:
        m = 1
        n = len(arr)

    return m, n


if __name__ == "__main__":

    x = np.arange(0, 10)

    plot(x, 2*x, 3*x, x, linestyle='dashed')
    plot(x, color='black')
    plot(x, np.array([x, 2*x]), color='green')
    plot(np.array([x, 2*x]), color='red')
    plot(np.random.rand(2, 10), color='blue')
    plot(np.random.rand(9, 10))
    # pdb.set_trace()
    plt.show()

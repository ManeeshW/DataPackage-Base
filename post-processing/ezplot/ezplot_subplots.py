from __future__ import print_function

import matplotlib as mpl
import matplotlib.pyplot as plt
import pdb

class SubPlot:
    ''' subplot class '''


    def __init__(self, rows, cols):

        if not isinstance(rows, int):
            print('ezplot error: Number of subplot rows must be an integer.')
            return

        if not isinstance(cols, int):
            print('ezplot error: Number of subplot columns must be an integer.')
            return

        self.rows = rows
        self.cols = cols
        self.max_ax = rows * cols

        self.line_mode = 0
        if rows==1 or cols==1:
            self.line_mode = 1

        self.fig, self.ax = plt.subplots(rows, cols)

        return


    def subplot(self, subplot_number_r, subplot_number_c=0):

        if not isinstance(subplot_number_r, int) or not isinstance(subplot_number_c, int):
            print('ezplot error: subplot index must be an integer.')
            return

        if self.line_mode:
            if subplot_number_c >= self.max_ax:
                print('ezplot error: subplot index must be less than max columns or rows')
                return
        else:
            if subplot_number_r >= self.rows:
                print('ezplot error: subplot row index must be less than max row')
                return

            if subplot_number_c >= self.cols:
                print('ezplot error: subplot column index must be less than max columns')
                return

        if subplot_number_c < 0 or subplot_number_r < 0:
            print('ezplot error: subplot indices must be a positive integers.')
            return

        if self.line_mode:
            self.ax_no = subplot_number_r
        else:
            self.ax_no = (subplot_number_r, subplot_number_c)

        return


    def suptitle(self, title):
        self.fig.suptitle(title, fontsize=12)
        return


    def plot(self, x, y, params=None):
        params = "'r', linewidth=2"

        if self.line_mode:
            line = self.ax[self.ax_no].plot(x[:], y[:])
        else:
            line = self.ax[self.ax_no].plot(x[:], y[:])

        return

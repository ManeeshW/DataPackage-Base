import argparse
from pdb import set_trace
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


class Data:
    def __init__(self, file_name):
        self.data = pd.read_csv(file_name, header=0, skipinitialspace=True)
    
    def load_data(self):
        
        data = self.data
        
        self.t = data['t']
        self.t_system = data['time']
        self.N = len(self.t)

        self.t_imu = data['t_imu']
        self.freq_imu = data['freq_imu']
        self.ypr = self.get_data_3x1('ypr')
        self.a = self.get_data_3x1('a')
        self.W = self.get_data_3x1('W')
        self.temperature = data['temperature']

        self.t_gps = data['t_gps']
        self.freq_gps = data['freq_gps']
        self.rtk_x = self.get_data_3x1('rtk_x')
        self.rtk_v = self.get_data_3x1('rtk_v')
        self.llh = self.get_data_3x1('llh')
        self.status = data['status']
        self.num_sats = data['num_sats']

        self.t_ane1 = data['t_ane1']
        self.freq_ane1 = data['freq_ane1']
        self.uvw_ane1 = self.get_data_3x1('uvw_ane1')
        self.temp_ane1 = data['temp_ane1']

        self.t_ane2 = data['t_ane2']
        self.freq_ane2 = data['freq_ane2']
        self.uvw_ane2 = self.get_data_3x1('uvw_ane2')
        self.temp_ane2 = data['temp_ane2']

        self.base_t_imu = data['base_t_imu']
        self.base_freq_imu = data['base_freq_imu']
        self.base_ypr = self.get_data_3x1('base_ypr')
        self.base_a = self.get_data_3x1('base_a')
        self.base_W = self.get_data_3x1('base_W')

        self.base_t_gps = data['base_t_gps']
        self.base_freq_gps = data['base_freq_gps']
        self.base_llh = self.get_data_3x1('base_llh')
        self.base_status = data['base_status']
        self.base_num_sats = data['base_num_sats']
        

    def get_data_3x1(self, var_name):
        return [self.data['{}_0'.format(var_name)],
                self.data['{}_1'.format(var_name)],
                self.data['{}_2'.format(var_name)]]


def plot_31_1(x, y, x_label='', y_label=['1', '2', '3'], title=''):
    fig, ax = plt.subplots(3, 1)
    fig.suptitle('{}'.format(title), fontsize=12)

    y_label_on = len(y_label)

    ax[0].plot(x[:], y[0][:], 'r')
    ax[0].set_ylabel('{}'.format(y_label[0]))
    ax[0].grid('on')

    ax[1].plot(x[:], y[1][:], 'r')
    ax[1].set_ylabel('{}'.format(y_label[1]))
    ax[1].grid('on')

    ax[2].plot(x[:], y[2][:], 'r')
    ax[2].set_ylabel('{}'.format(y_label[2]))
    ax[2].grid('on')

    return


def main(args):
    data = Data(args.file)
    data.load_data()

    t = data.t
    plot_31_1(t, data.ypr, title='YPR')
    plot_31_1(t, data.base_a, title='Base a')
    plot_31_1(t, data.base_W, title='Base W')
    plot_31_1(t, data.base_llh, title='Base LLH')

    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description=(
            'Reads and plots the save file of FDCL flight software.'))
    parser.add_argument(
        '-f', '--file',
        help='specify the file name, default FILE="../build/test.txt"')
    
    args = parser.parse_args()

    main(args)

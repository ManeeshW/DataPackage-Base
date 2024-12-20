import gi
import numpy as np
import os
import threading

from datetime import datetime

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, GLib, Gdk

import matplotlib
matplotlib.use('GTK3Agg')
from matplotlib.figure import Figure
# from matplotlib.backends.backend_gtk3agg \
#     import FigureCanvasGTK3Agg as FigureCanvas
from matplotlib.backends.backend_gtk3cairo \
    import FigureCanvasGTK3Cairo as FigureCanvas

from numpy import arange, pi, random, linspace
import matplotlib.cm as cm

from common_types import rover, base


class Gui():
    def __init__(self, t0):

        # threading.Thread.__init__(self)
        # self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._t0 = t0

        # self.window_plot = Gtk.Window()
        # self.window_plot.connect("delete-event", Gtk.main_quit)
        # self.window_plot.set_default_size(400, 400)
        # self.window_plot.set_title('Real-Time Plot')

        # self.fig = Figure(figsize=(5,5), dpi=100)
        # self.canvas = FigureCanvas(self.fig)

        # self.ax = self.fig.add_subplot(111, aspect='equal')

        # t = np.arange(0.0, 3.0, 0.01)
        # s = np.sin(2*np.pi*t)
        # self.ax.plot(t, s)

        # self.fig.canvas.draw()

        # self.sw = Gtk.ScrolledWindow()
        # self.window_plot.add(self.sw)
        # self.sw.set_border_width(10)

        # self.canvas.set_size_request(400, 400)
        # self.sw.add_with_viewport(self.canvas)

        # self.window_plot.show_all()


        self.builder = Gtk.Builder()
        self.builder.add_from_file("gui.glade")

        self.window = self.builder.get_object('window_main')
        self.window.set_title('Real-Time Data Monitoring')
        self.window.connect('destroy', Gtk.main_quit)
        # self.window.connect('key-press-event', self.on_key_press)

        self.lbl_t = self.builder.get_object('lbl_t')
        self.lbl_freq_imu = self.builder.get_object('lbl_freq_imu')
        self.lbl_freq_gps = self.builder.get_object('lbl_freq_gps')
        self.lbl_freq_ane1 = self.builder.get_object('lbl_freq_ane1')
        self.lbl_freq_ane2 = self.builder.get_object('lbl_freq_ane2')

        self.btn_close = self.builder.get_object('btn_close')
        self.btn_close.connect('clicked', self.on_btn_close_clicked)

        self.ypr = self.get_vbox('lbl_ypr')
        self.a = self.get_vbox('lbl_a')
        self.W = self.get_vbox('lbl_W')
        self.rtk = self.get_vbox('lbl_rtk')
        self.ane1 = self.get_vbox('lbl_ane_1')
        self.ane2 = self.get_vbox('lbl_ane_2')
        self.lbl_rtk_status = self.builder.get_object('lbl_rtk_status')
        self.lbl_sats = self.builder.get_object('lbl_sats')

        self.base_ypr = self.get_vbox('lbl_base_ypr')
        self.base_a = self.get_vbox('lbl_base_a')
        self.base_W = self.get_vbox('lbl_base_W')
        self.base_gps = self.get_vbox('lbl_base_gps')
        self.base_gps_status = self.builder.get_object('lbl_base_gps_stat')
        self.base_gps_num_sat = self.builder.get_object('lbl_base_sats')
        self.base_ane = self.get_vbox('lbl_base_ane')

        self.window.connect('destroy', Gtk.main_quit)
        self.window.show_all()


    def on_btn_close_clicked(self, *args):
        print('GUI: close button clicked')
        self._on = False
        base.on = False
        Gtk.main_quit()


    def end_thread(self):
        self._on = False


    def update_gui(self):
        t = (datetime.now() - self._t0).total_seconds()

        # tt = np.arange(0.0, 3.0, 0.01)
        # s = np.sin(2*np.pi*(tt + t))
        # self.ax.clear()
        # self.ax.plot(tt, s)

        
        self.lbl_t.set_text('t = {:0.1f} s'.format(t))
        self.lbl_freq_imu.set_text( \
            'IMU = {:3.0f} Hz'.format(rover.imu.freq))
        self.lbl_freq_gps.set_text(
            'GPS = {:3.0f} Hz'.format(rover.gps.freq))
        self.lbl_freq_ane1.set_text(
            'ANE1 = {:3.0f} Hz'.format(rover.ane1.freq))
        self.lbl_freq_ane2.set_text(
            'ANE2 = {:3.0f} Hz'.format(rover.ane2.freq))

        self.lbl_rtk_status.set_text(self.get_gps_status(rover.gps.status))
        self.lbl_sats.set_text('{:d}'.format(rover.gps.num_sats))

        self.update_vbox(self.ypr, rover.imu.ypr)
        self.update_vbox(self.a, rover.imu.a)
        self.update_vbox(self.W, rover.imu.W)
        self.update_vbox(self.rtk, rover.gps.rtk_x)
        self.update_vbox(self.ane1, rover.ane1.uvw)
        self.update_vbox(self.ane2, rover.ane2.uvw)

        self.update_vbox(self.base_ypr, base.imu.ypr)
        self.update_vbox(self.base_a, base.imu.a)
        self.update_vbox(self.base_W, base.imu.W)
        self.update_vbox(self.base_gps, base.gps.llh)
        self.base_gps_status.set_text(self.get_gps_status(base.gps.status))
        self.base_gps_num_sat.set_text('{:d}'.format(base.gps.num_sats))
        self.update_vbox(self.base_ane, base.ane1.uvw)

        return True


    def get_vbox(self, name):
        vbox = []
        for i in range(3):
            vbox.append(self.builder.get_object('{}_{}'.format(name, i + 1)))
        return vbox


    def update_vbox(self, vbox, data):
        for i in range(3):
            vbox[i].set_text('{:8.2f}'.format(data[i]))


    def get_grid(self, name):
        vbox = []
        for i in range(1, 4):
            for j in range(1, 4):
                vbox.append(self.builder.get_object('{}_{}{}'.format(name, \
                    i, j)))
        return vbox


    def update_grid(self, grid, data):
        for i in range(3):
            for j in range(3):
                k = 3 * i + j
                grid[k].set_text('{:8.2f}'.format(data[i, j]))

    
    def get_gps_status(self, status_code):
        if status_code == 0:
            return 'Invalid'
        elif status_code == 1:
            return 'SPP'
        elif status_code == 2:
            return 'DGNSS'
        elif status_code == 3:
            return 'RTK Float'
        elif status_code == 4:
            return 'RTK Fixed'
        elif status_code == 6:
            return 'SBAS'
        else:
            return 'No signal'


    # def run(self):
    #     print('GUI: starting thread')

    #     GLib.idle_add(self.update_gui)
    #     # self.daemon = True
    #     Gtk.main()

    #     base.on = False
    #     print('GUI: thread closed!')


# def thread_gui(thread_id, t0):
#     print('GUI: starting thread ..')

#     gui = Gui(thread_id, t0)
#     # GLib.idle_add(gui.update_gui)
#     Gtk.main()

#     # rover.on = False
#     print('GUI: thread closed!')

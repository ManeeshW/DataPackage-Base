import gi
import threading
import time

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, GLib, Gdk

from datetime import datetime

from base_gps import SwiftnavPiksiMulti
from base_imu import VectornavVn100
from common_types import base
from gui import Gui
from log import Log
from wifi import WifiClient

GPS_ON = False
IMU_ON = False


# These won't change, unless there is a hardware change.
GPS_PORT = '/dev/cu.usbserial-AK05WMX3'
IMU_PORT = '/dev/cu.usbserial-AK05WNDF'

def main():
    print('MAIN: Starting rover')

    t0 = datetime.now()

    gui = Gui(t0)

    thread_imu = VectornavVn100(1, IMU_PORT, 115200, t0, IMU_ON)
    thread_gps = SwiftnavPiksiMulti(2, GPS_PORT, 115200, t0, GPS_ON)
    thread_log = Log(3, t0)
    thread_wifi = WifiClient(4)
    
    threads = []
    threads.append(thread_imu)
    threads.append(thread_gps)
    threads.append(thread_log)
    threads.append(thread_wifi)

    for thread in threads:
        thread.start()

    GLib.idle_add(gui.update_gui)
    Gtk.main()
    
    print('MAIN: Sending close signal to threads')
    for thread in threads:
        thread.end_thread()
        thread.join()

    print('MAIN: Rover closed!')


if __name__ == '__main__':
    main()

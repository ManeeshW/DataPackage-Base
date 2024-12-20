import time
import serial
from datetime import datetime

# from common_types import ImuData, GpsData
from imu import ImuBno0555
from gps import SwiftnavPiksiMulti
from anemo import Anemo
from log import Log
from wifi import WifiServer

GPS_ON = True
IMU_ON = True 

ANE1_ON = True
ANE2_ON = True
MINI_ANEMO_MODE = True

# These won't change, unless there is a hardware change.
GPS_PORT = '/dev/ttyS0'
ANEMO1_PORT = '/dev/ttyUSB0'
ANEMO2_PORT = '/dev/ttyUSB1'


def main():
    print('MAIN: Starting rover')

    t0 = datetime.now()

    thread_imu = ImuBno0555(1, t0, IMU_ON)
    thread_gps = SwiftnavPiksiMulti(2, GPS_PORT, 115200, t0, GPS_ON)
    thread_ane1 = Anemo(3, ANEMO1_PORT, 9600, serial.SEVENBITS, serial.PARITY_EVEN, 1, t0, ANE1_ON, MINI_ANEMO_MODE)
    thread_ane2 = Anemo(4, ANEMO2_PORT, 9600, serial.SEVENBITS, serial.PARITY_EVEN, 2, t0, ANE2_ON, MINI_ANEMO_MODE)
    thread_log = Log(5, t0)
    thread_wifi = WifiServer(6)
    
    threads = []
    threads.append(thread_imu)
    threads.append(thread_gps)
    threads.append(thread_ane1)
    threads.append(thread_ane2)
    threads.append(thread_log)
    threads.append(thread_wifi)

    for thread in threads:
        thread.start()

    exit_flag = False

    while not exit_flag:
        try:
            time.sleep(0.01)

            if not thread_wifi.is_system_on():
                exit_flag = True

        except KeyboardInterrupt:
            exit_flag = True
            print('\nMAIN: Stopping rover')

    
    print('MAIN: Sending close signal to threads')
    for thread in threads:
        if thread._enabled:
            thread.end_thread()
            thread.join()

    print('MAIN: Rover closed!')


if __name__ == '__main__':
    main()

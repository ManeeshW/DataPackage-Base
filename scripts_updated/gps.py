from sbp.client.drivers.pyserial_driver import PySerialDriver
from sbp.client import Handler, Framer
from sbp.navigation import SBP_MSG_BASELINE_NED, SBP_MSG_POS_LLH, \
    SBP_MSG_VEL_NED, SBP_MSG_GPS_TIME, SBP_MSG_UTC_TIME
from sbp.system import SBP_MSG_HEARTBEAT

import argparse
import numpy as np
import threading

from datetime import datetime

from common_types import GpsData, rover


class SwiftnavPiksiMulti(threading.Thread):

    def __init__(self, thread_id, port, baud, t0, enabled, freq=5.0):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._enabled = enabled
        self._t0 = t0
        self._port = port
        self._baud = baud
        self._freq = freq
        self._new_data = True
        self._t = (datetime.now() - t0).total_seconds()
        self._no_rtk_counter = 5

        self._flag = 0
        self._n = 0.0
        self._e = 0.0
        self._d = 0.0
        self._lat = 0.0
        self._lon = 0.0
        self._h = 0.0
        self._v_n = 0.0
        self._v_e = 0.0
        self._v_d = 0.0
        self._wn = 0
        self._tow = 0
        self._utc_seconds = 0
        self._utc_nanos= 0
        self._num_sats = 0

        print('GPS: initialized')


    def whole_string(self):
        '''
        Returns all the data as a string
        '''

        return('%.0f\t%.0f\t%2.8f\t%2.8f\t%4.6f\t%6.0f\t%6.0f\t%6.0f\t'
               '%6.0f\t%6.0f\t%6.0f\t%.0f\t' %
               (self._wn, self._tow, self._lat, self._lon, self._h, self._n, 
               self._e, self._d, self._v_n, self._v_e, self._v_d, self._flag))


    def run(self):

        if not self._enabled:
            print('GPS: sensor has been disabled from settings'
                '\n\tExiting thread')
            return

        print('GPS: reading from {} at {}'.format(self._port, self._baud))

        freq = self._freq
        t = datetime.now()
        t_pre = datetime.now()
        avg_number = 100.0

        while self._on:
            with PySerialDriver(self._port, self._baud) as driver:
                with Handler(Framer(driver.read, None, verbose=False)) as source:
                    try:
                        msg_list = [SBP_MSG_BASELINE_NED, SBP_MSG_POS_LLH,
                                    SBP_MSG_VEL_NED, SBP_MSG_GPS_TIME, 
                                    SBP_MSG_HEARTBEAT, SBP_MSG_UTC_TIME]

                        if not self._on:
                            break

                        for msg, metadata in source.filter(msg_list):

                            if not self._on:
                                break

                            # LLH position in deg-deg-m
                            if msg.msg_type == 522:
                                t = datetime.now()
                                
                                dt = float((t - t_pre).total_seconds())
                                if dt <= 0:
                                    continue

                                freq = (freq * (avg_number - 1.0) + (1. / dt)) \
                                    / avg_number

                                t_pre = t

                                self._no_rtk_counter += 1

                                with self._lock:
                                    self._lat = msg.lat
                                    self._lon = msg.lon
                                    self._h = msg.height
                                    self._num_sats = int(msg.n_sats)

                                    if self._no_rtk_counter > 5:
                                        self._flag = int(msg.flags)
                                        self._no_rtk_counter = 6

                                    self._t = (t - self._t0).total_seconds()
                                    self._freq = int(freq)
                                    self._new_data = True

                                    self.update_data()

                            # RTK position in m
                            elif msg.msg_type == 524:
                                with self._lock:
                                    self._no_rtk_counter = 0
                                    self._flag = int(msg.flags)
                                    self._n = msg.n / 1.0e3
                                    self._e = msg.e / 1.0e3
                                    self._d = msg.d / 1.0e3

                            # RTK velocity in m/s
                            elif msg.msg_type == 526:
                                with self._lock:
                                    self._v_n = msg.n / 1.0e3
                                    self._v_e = msg.e / 1.0e3
                                    self._v_d = msg.d / 1.0e3

                            # GPS time
                            elif msg.msg_type == 258:
                                with self._lock:
                                    self._wn = msg.wn
                                    self._tow = msg.tow  # in millis

                            # UTC time
                            elif msg.msg_type == 259:
                                
                                with self._lock:
                                    self._utc_seconds = int(datetime(msg.year, msg.month, msg.day, msg.hours, msg.minutes, msg.seconds, 0).timestamp())
                                    self._utc_nanos= msg.ns
                                    timestamp_dt = datetime.fromtimestamp(self._utc_seconds + self._utc_nanos/1e9)
                                    #print(timestamp_dt)

                            else:
                                pass

                    except:
                        print('GPS: error reading message')

            print('GPS: thread closed')


    def update_data(self):
            data = GpsData(
                self._t,
                self._freq,
                np.array([self._n, self._e, self._d]),
                np.array([self._v_n, self._v_e, self._v_d]),
                np.array([self._lat, self._lon, self._h]),
                self._flag,
                self._num_sats,
                self._utc_seconds,
                self._utc_nanos
                )
            rover.update_gps(data)


    def get_data(self):
        with self._lock:
            new_data = self._new_data
            self._new_data = False

            return (new_data,
                GpsData(
                    self._t,
                    self._freq,
                    np.array([self._n, self._e, self._d]),
                    np.array([self._v_n, self._v_e, self._v_d]),
                    np.array([self._lat, self._lon, self._h]),
                    self._flag,
                    self._num_sats,
                    self._utc_seconds,
                    self._utc_nanos
                )
            )


    def end_thread(self):
        self._on = False
        print('GPS: thread close signal received')

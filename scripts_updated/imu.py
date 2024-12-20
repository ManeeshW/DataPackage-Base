import board
import adafruit_bno055
import numpy as np
import threading
import time

from datetime import datetime

from common_types import ImuData, rover


def to_numpy(data):
    x, y, z = data
    return np.array([x, y, z])


class ImuBno0555(threading.Thread):
    def __init__(self, thread_id, t0, enabled, freq=5.0):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._enabled = enabled
        self._t0 = t0

        i2c = board.I2C()
        self._sensor = adafruit_bno055.BNO055_I2C(i2c)
        self._sensor.mode = adafruit_bno055.NDOF_MODE
        self._last_val = 0xFFFF

        self._t = (datetime.now() - t0).total_seconds()
        self._ypr = np.zeros(3)
        self._a = np.zeros(3)
        self._W = np.zeros(3)
        self._temperature = 0.0
        self._freq = 0

        self._desired_dt = 1.0 / freq
        self._new_data = False

        print('IMU: initialized')


    def get_temperature(self):
        result = self._sensor.temperature
        if abs(result - self._last_val) == 128:
            result = self._sensor.temperature
            if abs(result - self._last_val) == 128:
                return 0b00111111 & result
        self._last_val = result
        return result


    def run(self):

        if not self._enabled:
            print('IMU: sensor has been disabled from settings'
                  '\n\tExiting thread')
            return

        print('IMU: starting thread')

        freq = self._freq
        t = datetime.now()
        t_pre = datetime.now()
        avg_number = 100.0

        while self._on:

            t = datetime.now()
            dt = (t - t_pre).total_seconds()
            if dt < self._desired_dt:
                continue

            freq = (freq * (avg_number - 1) + (1.0 / dt)) / avg_number
            t_pre = t

            ypr = to_numpy(self._sensor.euler)
            a = to_numpy(self._sensor.linear_acceleration)
            W = to_numpy(self._sensor.gyro)
            temperature = self.get_temperature()

            with self._lock:
                self._t = (t - self._t0).total_seconds()
                self._new_data = True
                self._freq = int(freq)
                self._ypr = ypr.astype(float)
                self._a = a.astype(float)
                self._W = W
                self._temperature = temperature

                self.update_data()

            # print(ypr.T)

        print('IMU: thread closed')

    
    def update_data(self):
        data = ImuData(
            self._t,
            self._freq,
            self._ypr,
            self._a,
            self._W,
            self._temperature
        )
        rover.update_imu(data)


    def get_data(self):
        with self._lock:
            new_data = self._new_data
            self._new_data = False

            return (new_data,
                ImuData(
                    self._t,
                    self._freq,
                    self._ypr,
                    self._a,
                    self._W,
                    self._temperature
                )
            )


    def end_thread(self):
        self._on = False
        print('IMU: thread close signal received')

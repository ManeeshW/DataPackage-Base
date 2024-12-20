import numpy as np
import threading
from datetime import datetime

class Time:
    def __init__(self, t=0.0):
        self._t0 = t
        self.t = (datetime.now()- self._t0).total_seconds()

class ImuData:
    def __init__(self, t=0.0, freq=0, ypr=np.zeros(3), a=np.zeros(3), \
            W=np.zeros(3), temperature=0.0):
        self.t = t
        self.freq = freq
        self.ypr = ypr
        self.a = a
        self.W = W
        self.temperature = temperature

class GpsData:
    def __init__(self, t=0, freq=0, rtk_x=np.zeros(3), rtk_v=np.zeros(3), \
            llh=np.zeros(3), status=0, num_sats=0, utc_seconds=0,utc_nanos=0):
        self.t = t
        self.freq = freq
        self.rtk_x = rtk_x
        self.rtk_v = rtk_v
        self.llh = llh
        self.status = status
        self.num_sats = num_sats
        self.utc_seconds = utc_seconds
        self.utc_nanos= utc_nanos


class AnemoData:
    def __init__(self, t=0.0, freq=0, uvw=np.zeros(3), temperature=0.0):
        self.t = t
        self.freq = freq
        self.uvw = uvw
        self.temperature = temperature

class Cam1Data:
    def __init__(self, t=0.0, freq=0, idx=0):
        self.t = t
        self.freq = freq
        self.idx = idx

class Cam2Data:
    def __init__(self, t_stamp=0.0, t=0.0, freq=0, idx=0):
        self.t_stamp = t_stamp
        self.t = t
        self.freq = freq
        self.idx = idx

class Data:
    def __init__(self):
        self._lock = threading.Lock()
        self.on = True
        self.imu = ImuData()
        self.gps = GpsData()
        self.ane1 = AnemoData()
        self.ane2 = AnemoData()
        self.cam1 = Cam1Data()
        self.cam2 = Cam2Data()

    def update_imu(self, data):
        with self._lock:
            self.imu = data

    def update_gps(self, data):
        with self._lock:
            self.gps = data

    def update_ane(self, index, data):
        if index == 1:
            with self._lock:
                self.ane1 = data
        elif index == 2:
            with self._lock:
                self.ane2 = data
        else:
            raise('DATA: currently this code only supports 2 anemometers')

    def update_cam1(self, data):
        with self._lock:
            self.cam1 = data

    def update_cam2(self, data):
        with self._lock:
            self.cam2 = data

rover = Data()
base = Data()

import numpy as np
import threading
import time

from datetime import datetime

from common_types import ImuData, GpsData, AnemoData, Cam1Data, Cam2Data, rover, base


class Log(threading.Thread):
    def __init__(self, thread_id, t0, freq=50):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()
        self._enabled = True

        self._on = True
        self._header_written = False
        self._t0 = t0

        self._section = 0
        self._file_name = datetime.now().strftime('data_logs/log_%Y%m%d_%H%M%S')
        self._f = open('{}_{}.txt'.format(self._file_name, self._section), 'w')
        with open('recentLogFile.txt', 'w') as f:
            f.write(self._file_name)
        self._desired_dt = 1.0 / freq
        self._freq = 0

        self._imu = ImuData()
        self._gps = GpsData()
        self._ane1 = AnemoData()
        self._ane2 = AnemoData()
        self._cam1 = Cam1Data()
        self._cam2 = Cam2Data()

        self._base_imu = ImuData()
        self._base_gps = GpsData()
        self._base_ane = AnemoData()

        print('LOG: initialized')

    
    def start_new_log_section(self):
        self._f.close()

        self._section += 1
        self._f = open('{}_{}.txt'.format(self._file_name, self._section), 'w')
        self._header_written = False


    def run(self):
        print('LOG: starting thread')
        t_microseconds = 0.0
        freq = 0.0

        t_pre = datetime.now()

        avg_number = 50
    
        while self._on:

            t = datetime.now()
            dt = (t - t_pre).total_seconds()
            if dt < self._desired_dt:
                time.sleep(0.01)
                continue

            self._freq = int((freq * (avg_number - 1) + (1 / dt)) / avg_number)
            t_pre = t

            dt_millis = t - self._t0
            t_millis = int(dt_millis.seconds * 1.0e3 \
                + dt_millis.microseconds / 1.0e3)
            t_microseconds += dt_millis.microseconds / 1.0e3
            self.update_data()

            #with self._lock:
            if not self._header_written:
                self.write_header()
                
            self.write_data(t_millis)
            #self.write_data(t_microseconds)
                

        self._f.close()

        print('LOG: thread closed')

    def update_data(self):
        self.update_imu_data()
        self.update_gps_data()
        self.update_ane_data(1)
        self.update_ane_data(2)
        self.update_base_imu_data()
        self.update_base_gps_data()
        self.update_base_ane_data(1)
        #self.update_base_ane_data(1)
        self.update_cam1_data()
        self.update_cam2_data()
      

    def update_imu_data(self):
        with self._lock:
            self._imu = rover.imu


    def update_gps_data(self):
        with self._lock:
            self._gps = rover.gps


    def update_ane_data(self, index):
        if index == 1:
            with self._lock:
                self._ane1 = rover.ane1

        elif index == 2:
            with self._lock:
                self._ane2 = rover.ane2
        
        else:
            raise('LOG: currently this code only supports 2 anemometers')

    def update_base_imu_data(self):
        with self._lock:
            self._base_imu = base.imu


    def update_base_gps_data(self):
        with self._lock:
            self._base_gps = base.gps

    def end_thread(self):
        self._on = False
        print('LOG: thread close signal received')

    def update_base_ane_data(self, index):
        if index == 1:
            with self._lock:
                self._base_ane = base.ane1

        elif index == 2:
            with self._lock:
                self._base_ane = base.ane2
        
        else:
            raise('LOG: currently this code only supports 2 anemometers')  

    def update_cam1_data(self):
        with self._lock:
            self._cam1 = rover.cam1

    def update_cam2_data(self):
        with self._lock:
            self._cam2 = rover.cam2
    
    def write_header(self):
        # NOTE: header order and the data order must be of the same order.
        self._f.write('t, ')
        self._f.write('t_system, ')
        self._f.write('base_utc_sec, ')
        self._f.write('base_utc_nano, ')

        self._f.write(self.string_vector('base_ypr'))
        self._f.write(self.string_vector('base_a'))
        self._f.write(self.string_vector('base_W'))
        self._f.write(self.string_vector('base_llh'))
        self._f.write('base_status, ')
        self._f.write('base_sats, ')
        self._f.write(self.string_vector('base_ane'))

        self._f.write(self.string_vector('ypr'))
        self._f.write(self.string_vector('a'))
        self._f.write(self.string_vector('W'))
        self._f.write(self.string_vector('rtk_x'))
        self._f.write(self.string_vector('rtk_v'))
        self._f.write(self.string_vector('llh'))
        self._f.write('status, ')
        self._f.write('sats, ')
        self._f.write('utc_sec, ')
        self._f.write('utc_nano, ')

        self._f.write(self.string_vector('ane1'))
        self._f.write(self.string_vector('ane2'))
        self._f.write('idx1, ')
        self._f.write('idx2, ')

        self._f.write('\n')
        self._header_written = True


    def write_data(self, t_millis):
        # NOTE: header order and the data order must be of the same order.
        self.write_scalar(t_millis/1000)
        self.write_scalar(datetime.now().strftime('%H%M%S'))
        self.write_scalar(self._base_gps.utc_seconds)
        self.write_scalar(self._base_gps.utc_nanos)

        self.write_vector(self._base_imu.ypr)
        self.write_vector(self._base_imu.a)
        self.write_vector(self._base_imu.W)
        self.write_vector(self._base_gps.llh)
        self.write_scalar(self._base_gps.status)
        self.write_scalar(self._base_gps.num_sats)
        self.write_vector(self._base_ane.uvw)

        self.write_vector(self._imu.ypr)
        self.write_vector(self._imu.a)
        self.write_vector(self._imu.W)
        self.write_vector(self._gps.rtk_x)
        self.write_vector(self._gps.rtk_v)
        self.write_vector(self._gps.llh)
        self.write_scalar(self._gps.status)
        self.write_scalar(self._gps.num_sats)
        self.write_scalar(self._gps.utc_seconds)
        self.write_scalar(self._gps.utc_nanos)
        self.write_vector(self._ane1.uvw)
        self.write_vector(self._ane2.uvw)
        self.write_scalar(self._cam1.idx)
        self.write_scalar(self._cam2.idx)




        # self.write_scalar(self._imu.t)
        # self.write_scalar(self._imu.freq)
        # self.write_scalar(self._imu.temperature)
        # self.write_scalar(self._gps.t)
        # self.write_scalar(self._gps.freq)
        # self.write_scalar(self._ane1.t)
        # self.write_scalar(self._ane1.freq)
        
        # self.write_scalar(self._ane1.temperature)
        # self.write_scalar(self._ane2.t)
        # self.write_scalar(self._ane2.freq)
        # self.write_scalar(self._ane2.temperature)
        # self.write_scalar(self._base_imu.t)
        # self.write_scalar(self._base_imu.freq)
        # self.write_scalar(self._base_imu.temperature)

        # self.write_scalar(self._base_gps.t)
        # self.write_scalar(self._base_gps.freq)
        # self.write_scalar(self._base_ane.t)
        # self.write_scalar(self._base_ane.freq)
        # self.write_scalar(self._base_ane.temperature)

        self._f.write('\n')


    def string_vector(self, name, length=3):
        out = ''
        for i in range(length):
            out += '{}_{},'.format(name, i)
        return out


    def string_3x3(self, name):
        out = ''
        for i in range(3):
            for j in range(3):
                out += '{}_{}{},'.format(name, i, j)
        return out


    def write_scalar(self, data):
        self._f.write('{},'.format(data))


    def write_vector(self, data, length=3):
        line = ''
        for i in range(length):
            line += '{},'.format(data[i])
        self._f.write(line)


    def write_3x3(self, data):
        for i in range(3):
            for j in range(3):
                self._f.write('{},'.format(data[i, j]))

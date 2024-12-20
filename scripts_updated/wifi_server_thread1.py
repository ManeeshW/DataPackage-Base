import numpy as np
import socket
import threading
import time

from datetime import datetime

import proto.data_pb2
from common_types import ImuData, GpsData, AnemoData, rover

HOST = '192.168.8.1'
PORT = 5000
#HOST = '192.168.10.4'
#PORT = 9999i

class ServerThread1(threading.Thread):
    def __init__(self, thread_id, conn, addr, freq=100):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()
        self.conn = conn

        self._on = True
        self._system_on = True
        self._enabled = True

        self._desired_dt = 1.0 / freq
        self._freq = 0

        self._client_data = proto.data_pb2.Data()
        self._server_data = proto.data_pb2.Data()
        print('WIFI: connected by ', addr)
        print('WIFI: initialized')
        

    def run(self):
        print('WIFI: starting thread')

        freq = 0.0

        t0 = datetime.now()
        t_pre = datetime.now()
        rover.cam2.t_stamp = t_pre
        print('Time stamp of the RPI sent : ', rover.cam2.t_stamp)

        avg_number = 100
                
        while self._on:
            t = datetime.now()
            dt = (t - t_pre).total_seconds()
            if dt < self._desired_dt:
                continue

            dt_millis = t - t0
            t_millis = int(dt_millis.seconds * 1.0e3
                        + dt_millis.microseconds / 1.0e3)
            
            try:
                client_data = self.conn.recv(1024)
                if not client_data:
                    break
            except ConnectionResetError:
                print('WIFI: server closed')
                break
            except:
                print('WIFI: error reading receiving from client')

            self.parse_received_data(client_data)

            data_to_send = self.get_data_to_send(t_millis)
            self.conn.sendall(data_to_send)

        print('WIFI: thread closed')

    
    def get_data_to_send(self, t_millis):
        self._server_data.system_on = self._on
        self._server_data.t = t_millis
        self._server_data.t_system = datetime.now().utcnow().timestamp()

        self.update_imu_data()
        self.update_gps_data()
        self.update_ane_data(1)
        self.update_ane_data(2)
        self.update_cam1_data()
        self.update_cam2_data()

        return self._server_data.SerializeToString()

    
    def parse_received_data(self, data):
        self._client_data.ParseFromString(data)
        self._system_on = self._client_data.system_on

    
    def is_system_on(self):
        return self._system_on


    def update_imu_data(self):
        with self._lock:
            self._server_data.t_imu = rover.imu.t
            self._server_data.freq_imu = rover.imu.freq
            self._server_data.ypr.x = rover.imu.ypr[0]
            self._server_data.ypr.y =rover.imu.ypr[1]
            self._server_data.ypr.z =rover.imu.ypr[2]
            self._server_data.a.x = rover.imu.a[0]
            self._server_data.a.y = rover.imu.a[1]
            self._server_data.a.z = rover.imu.a[2]
            self._server_data.W.x = rover.imu.W[0]
            self._server_data.W.y = rover.imu.W[1]
            self._server_data.W.z = rover.imu.W[2]
            self._server_data.temperature = rover.imu.temperature

    
    def update_gps_data(self):
        with self._lock:
            self._server_data.t_gps = rover.gps.t
            self._server_data.freq_gps = rover.gps.freq
            self._server_data.rtk_x.x = rover.gps.rtk_x[0]
            self._server_data.rtk_x.y = rover.gps.rtk_x[1]
            self._server_data.rtk_x.z = rover.gps.rtk_x[2]
            self._server_data.rtk_v.x = rover.gps.rtk_v[0]
            self._server_data.rtk_v.y = rover.gps.rtk_v[1]
            self._server_data.rtk_v.z = rover.gps.rtk_v[2]
            self._server_data.llh.x = rover.gps.llh[0]
            self._server_data.llh.y = rover.gps.llh[1]
            self._server_data.llh.z = rover.gps.llh[2]
            self._server_data.status = rover.gps.status
            self._server_data.num_sats = rover.gps.num_sats
            self._server_data.utc_seconds = rover.gps.utc_seconds
            self._server_data.utc_nanos = rover.gps.utc_nanos

    
    def update_ane_data(self, index):
        if index == 1:
            with self._lock:
                self._server_data.t_ane1 = rover.ane1.t
                self._server_data.freq_ane1 = rover.ane1.freq
                self._server_data.uvw_ane1.x = rover.ane1.uvw[0]
                self._server_data.uvw_ane1.y = rover.ane1.uvw[1]
                self._server_data.uvw_ane1.z = rover.ane1.uvw[2]
                self._server_data.temp_ane1 = rover.ane1.temperature
        elif index == 2:
            with self._lock:
                self._server_data.t_ane2 = rover.ane2.t
                self._server_data.freq_ane2 = rover.ane2.freq
                self._server_data.uvw_ane2.x = rover.ane2.uvw[0]
                self._server_data.uvw_ane2.y = rover.ane2.uvw[1]
                self._server_data.uvw_ane2.z = rover.ane2.uvw[2]
                self._server_data.temp_ane2 = rover.ane2.temperature
        else:
            raise('WIFI: currently this code only supports 2 anemometers')

    def update_cam1_data(self):
        with self._lock:
            self._server_data.t_cam1 = rover.cam1.t
            self._server_data.freq_cam1 = rover.cam1.freq
            self._server_data.idx_cam1 = rover.cam1.idx

    def update_cam2_data(self):
        with self._lock:
            self._server_data.t_cam2 = rover.cam2.t
            self._server_data.freq_cam2 = rover.cam2.freq
            self._server_data.idx_cam2 = rover.cam2.idx

    def end_thread(self):
        self._on = False

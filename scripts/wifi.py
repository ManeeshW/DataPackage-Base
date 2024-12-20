import numpy as np
import socket
import threading
import time

from datetime import datetime

import proto.data_pb2
from common_types import ImuData, GpsData, AnemoData, rover, base


HOST = '192.168.8.1'
PORT = 5000

#HOST = '192.168.10.4'
#PORT = 9999


class WifiClient(threading.Thread):
    def __init__(self, thread_id, freq=100):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._system_on = True
        self._enabled = True

        self._desired_dt = 1.0 / freq
        self._freq = 0

        self._client_data = proto.data_pb2.Data()
        self._server_data = proto.data_pb2.Data()

        print('WIFI: initialized')


    def run(self):
        print('WIFI: starting thread')

        freq = 0.0

        t0 = datetime.now()
        t_pre = datetime.now()

        avg_number = 100

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))

            while self._on:
                t = datetime.now()
                dt = (t - t_pre).total_seconds()
                if dt < self._desired_dt:
                    continue
                
                dt_millis = t - t0
                t_millis = int(dt_millis.seconds * 1.0e3
                            + dt_millis.microseconds / 1.0e3)

                data_to_send = self.get_data_to_send(t_millis)
                s.sendall(data_to_send)

                data_received = s.recv(1024)
                if not data_received:
                    break
                self.parse_received_data(data_received)
                self.update_data()

            print('WIFI: sending turn off cammand to rover')
            for i in range(10):
                self._client_data.system_on = False
                data_to_send = self.get_data_to_send(t_millis)

                try:
                    s.sendall(data_to_send)
                except BrokenPipeError:
                    break
                except:
                    print('WIFI: error reading sending data to server')
                time.sleep(0.1)

        print('WIFI: thread closed')

    
    def get_data_to_send(self, t_millis):
        with self._lock:
            self._client_data.system_on = self._on

        return self._client_data.SerializeToString()

    
    def parse_received_data(self, data):
        self._server_data.ParseFromString(data)
        #print(self._server_data)

    
    def update_data(self):
        imu = ImuData(
            self._server_data.t_imu,
            self._server_data.freq_imu,
            [self._server_data.ypr.x, self._server_data.ypr.y, \
                self._server_data.ypr.z],
            [self._server_data.a.x, self._server_data.a.y, \
                self._server_data.a.z],
            [self._server_data.W.x, self._server_data.W.y, \
                self._server_data.W.z],
            self._server_data.temperature
        )

        gps = GpsData(
            self._server_data.t_gps,
            self._server_data.freq_gps,
            [self._server_data.rtk_x.x, self._server_data.rtk_x.y, \
                self._server_data.rtk_x.z],
            [self._server_data.rtk_v.x, self._server_data.rtk_v.y, \
                self._server_data.rtk_v.z],
            [self._server_data.llh.x, self._server_data.llh.y, \
                self._server_data.llh.z],
            self._server_data.status,
            self._server_data.num_sats
        )

        ane1 = AnemoData(
            self._server_data.t_ane1,
            self._server_data.freq_ane1,
            [self._server_data.uvw_ane1.x, self._server_data.uvw_ane1.y, \
                self._server_data.uvw_ane1.z],
            self._server_data.temp_ane1
        )
        
        ane2 = AnemoData(
            self._server_data.t_ane2,
            self._server_data.freq_ane2,
            [self._server_data.uvw_ane2.x, self._server_data.uvw_ane2.y, \
                self._server_data.uvw_ane2.z],
            self._server_data.temp_ane2
        )

        rover.update_imu(imu)
        rover.update_gps(gps)
        rover.update_ane(1, ane1)
        rover.update_ane(2, ane2)


    def end_thread(self):
        self._client_data.system_on = False
        time.sleep(2)
        self._on = False



class WifiServer(threading.Thread):
    def __init__(self, thread_id, freq=100):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._system_on = True
        self._enabled = True

        self._desired_dt = 1.0 / freq
        self._freq = 0

        self._client_data = proto.data_pb2.Data()
        self._server_data = proto.data_pb2.Data()

        print('WIFI: initialized')


    def run(self):
        print('WIFI: starting thread')

        freq = 0.0

        t0 = datetime.now()
        t_pre = datetime.now()

        avg_number = 100

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((HOST, PORT))
            s.listen()
            conn, addr = s.accept()

            with conn:
                print('WIFI: connected by ', addr)
                
                while self._on:
                    t = datetime.now()
                    dt = (t - t_pre).total_seconds()
                    if dt < self._desired_dt:
                        continue

                    dt_millis = t - t0
                    t_millis = int(dt_millis.seconds * 1.0e3
                                + dt_millis.microseconds / 1.0e3)
                    
                    try:
                        client_data = conn.recv(1024)
                        if not client_data:
                            break
                    except ConnectionResetError:
                        print('WIFI: server closed')
                        break
                    except:
                        print('WIFI: error reading receiving from client')

                    self.parse_received_data(client_data)

                    data_to_send = self.get_data_to_send(t_millis)
                    conn.sendall(data_to_send)

        print('WIFI: thread closed')

    
    def get_data_to_send(self, t_millis):
        self._server_data.system_on = self._on
        self._server_data.t = t_millis
        self._server_data.t_system = datetime.now().utcnow().timestamp()

        self.update_imu_data()
        self.update_gps_data()
        self.update_ane_data(1)
        self.update_ane_data(2)

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
            self._server_data.ypr.y = rover.imu.ypr[1]
            self._server_data.ypr.z = rover.imu.ypr[2]
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


    def end_thread(self):
        self._on = False

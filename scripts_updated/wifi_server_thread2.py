import numpy as np
import socket
import threading
import time

from datetime import datetime
from google.protobuf.timestamp_pb2 import Timestamp
import proto.data_pb2
import proto.time_pb2
from common_types import rover


class ServerThread2(threading.Thread):
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
        self.i0 = 1
        #self._client_data = proto.data_pb2.Data()
        #self._server_data = proto.data_pb2.Data()
        self._client_data2 = proto.time_pb2.Data()
        self._server_data2 = proto.time_pb2.Data()
        print('WIFI: connected by ', addr)
        print('WIFI: initialized')
        #print(datetime.now().utcnow().timestamp())

    def run(self):
        print('WIFI server 2 starting thread')
        print("Time test 1 : ", type(rover.cam2.t_stamp))
        print(datetime)
        print(type(datetime.now()))
        freq = 0.0

        t0 = datetime.now()
        t_pre = datetime.now()

        avg_number = 100

        
                
        while self._on:
            ## Test communication --------------
            
            # if not data:
            #     break
            data = b"Hello, camera"
            data_to_send = self.get_data_to_send()
            self.conn.sendall(data_to_send)
            

            client_data2 = self.conn.recv(1024)
            self.parse_received_data(client_data2)

            #data_to_send = self.get_data_to_send()
            #self.conn.sendall(data_to_send)
            #print(f"Received {client_data2}")
            ##----------------------------------
            
            # t = datetime.now()
            # dt = (t - t_pre).total_seconds()
            # if dt < self._desired_dt:
            #     continue
            
            # dt_millis = t - t0
            # t_millis = int(dt_millis.seconds * 1.0e3
            #             + dt_millis.microseconds / 1.0e3)

            # try:
            #     client_data2 = self.conn.recv(1024)
            #     if not client_data2:
            #         break

            # except ConnectionResetError:
            #     print('WIFI: server closed')
            #     break
            # except:
            #     print('WIFI: error reading receiving from client')

            # print("looping")
            
            # try:
                
            #     data_to_send = self.get_data_to_send()
            #     self.conn.sendall(data_to_send)

            #     print("try ..3  conn.recv(1024)")
            #     client_data2 = self.conn.recv(2048)
            #     if not client_data2:
            #         break
                
            #     print("try ..4  parse_received_data(client_data2)")
            #     self.parse_received_data(client_data2)
            #     print("self.conn.recv(1024) ..0")

            # except ConnectionResetError:
            #     print('WIFI: server closed')
            #     break
            # except:
            #     print('WIFI: error reading receiving from client')
            
        
        print('WIFI: thread closed')
    
    def get_data_to_send(self):
        self._server_data2.system_on = self._on
        #self._server_data2.t = t_millis
        #print("Time test 2 : ", rover.cam2.t_stamp)
        t = datetime.now().timestamp()
        seconds = int(t)
        nanos = int(t % 1 * 1e9)
        #proto_timestamp = Timestamp(seconds=seconds, nanos=nanos)
        proto_timestamp = Timestamp()
        proto_timestamp.GetCurrentTime()
        
        if self.i0 == 1:
            print("#######################")
            print("seconds : ", seconds)
            print("nanos : ", nanos)
            timestamp_dt = datetime.fromtimestamp(seconds + nanos/1e9)
            print(timestamp_dt)
            self.i0 = 0

        # print(proto_timestamp)
        
        self._server_data2.t_stamp_seconds = seconds # rover.cam2.t_stamp #.utcnow().timestamp()
        self._server_data2.t_stamp_nanos = nanos
        
        return self._server_data2.SerializeToString()

    
    def parse_received_data(self, data):
        self._client_data2.ParseFromString(data)
        self._system_on = self._client_data2.system_on
        self.update_cam2_data()
    
    def is_system_on(self):
        return self._system_on

    def update_cam2_data(self):
        with self._lock:
            rover.cam2.t = self._client_data2.t_cam2
            rover.cam2.freq = self._client_data2.freq_cam2
            rover.cam2.idx = self._client_data2.idx_cam2 
            #print(rover.cam2.idx)

    def end_thread(self):
        self._on = False

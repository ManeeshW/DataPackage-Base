from common_types import ImuData, rover

import numpy as np
import serial
import struct
import threading
import time

from array import array
from datetime import datetime

'''
First, make sure IMU is configured correctly.
$VNASY,0*XX                 // stop async message printing
$VNWRG,06,0*XX              // stop ASCII message outputs
$VNWRG,75,2,80,01,0128*XX   // output binary message at 10 Hz
$VNCMD*XX                   // enter command mode
system save                 // save settings to flash memory
exit                        // exit command mode
$VNASY,1*XX                 // resume async message printing
'''

class VectornavVn100(threading.Thread):

    def __init__(self, thread_id, port, baud, t0, enabled, freq=25):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._enabled = enabled
        self._t0 = t0
        self._port = port
        self._baud = baud
        self._freq = freq

        self._t = (datetime.now() - t0).total_seconds()
        self._desired_dt = 1.0 / float(freq)

        self._ypr = np.zeros(3)
        self._a = np.zeros(3)
        self._W = np.zeros(3)
        

        print('IMU: initialized')


    def run(self):

        if not self._enabled:
            print('IMU: sensor has been disabled from settings'
                  '\n\tExiting thread')
            return

        print('IMU: reading from {} at {}'.format(self._port, self._baud))

        # in case the port is not properly closed
        try:
            temp = serial.Serial(self._port, self._baud)
            temp.close()
        except:
            print('\033[91m' + 'Unable to open IMU port at ' + self._port
                  + ':' + str(self._baud) + '\033[0m')
            return

        print('IMU: starting main loop')

        freq = self._freq
        t = datetime.now()
        t_pre = datetime.now()
        avg_number = 100.0

        with serial.Serial(self._port, self._baud, timeout=1) as s:
            
            # Clear the buffer first.
            print('IMU: clearing buffer')
            num_bytes = s.in_waiting
            s.read(num_bytes)

            print('IMU: starting main loop')
            while self._on:
                imu_sync_detected = False

                num_bytes = s.in_waiting
                if num_bytes == 0:
                    continue
                
                imu_sync_detected = self.check_sync_byte(s)
                if not imu_sync_detected:
                    continue

                success = self.read_imu_data(s)
                
                if not success:
                    continue

                with self._lock:
                    self.update_data()

        print('IMU: thread closed')

    
    def check_sync_byte(self, s):
        for _ in range(s.in_waiting):
            byte_in = s.read(1)
            int_in = int.from_bytes(byte_in, 'little')
            if int_in == 250:
                return True


    def read_imu_data(self, s):
        # Read data.
        data = s.read(41)
        
        # Check if there are unexpected errors in the message.
        checksum_array = array('B', [data[40], data[39]])
        checksum = struct.unpack('H', checksum_array)[0]

        crc = self.calculate_imu_crc(data[:39])

        if not crc == checksum:
            print('IMU: CRC error')
            return False

        return self.parse_data(data)


    def parse_data(self, data):
        try:
            self._ypr[0] = struct.unpack('f', data[3:7])[0]
            self._ypr[1] = struct.unpack('f', data[7:11])[0]
            self._ypr[2] = struct.unpack('f', data[11:15])[0]
            self._W[0] = struct.unpack('f', data[15:19])[0]
            self._W[1] = struct.unpack('f', data[19:23])[0]
            self._W[2] = struct.unpack('f', data[23:27])[0]
            self._a[0] = struct.unpack('f', data[27:31])[0]
            self._a[1] = struct.unpack('f', data[31:35])[0]
            self._a[2] = struct.unpack('f', data[35:39])[0]
        except:
            print('IMU: error parsing data')
            return False
        
        return True

    
    # Calculate the 16-bit CRC for the given message.
    def calculate_imu_crc(self, data):
        data = bytearray(data)

        crc = np.array([0], dtype=np.ushort)
        for i in range(len(data)):
            crc[0] = (crc[0] >> 8) | (crc[0] << 8)
            crc[0] ^= data[i]
            crc[0] ^= (crc[0] & 0xff) >> 4
            crc[0] ^= crc[0] << 12
            crc[0] ^= (crc[0] & 0x00ff) << 5
        
        return crc[0]


    def update_data(self):
        data = ImuData(
            self._t,
            self._freq,
            self._ypr,
            self._a,
            self._W
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
                )
            )

    def end_thread(self):
        self._on = False
        print('IMU: thread close signal received')

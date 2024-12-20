import serial
import numpy as np
import threading
import time

from datetime import datetime

from common_types import AnemoData, rover


class Anemo(threading.Thread):

    def __init__(self, thread_id, port, baud, index, t0, enabled, mini_mode, \
            freq=10.0):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._enabled = enabled
        self._mini_mode = mini_mode
        self._t0 = t0
        self._index = index
        self._port = port
        self._baud = baud
        self._freq = 0
        self._desired_dt = 1.0 / freq
        self._new_data = False

        self._t = datetime.now()
        self._uvw = np.zeros(3)
        self._temperature = 0.0

        print('ANEMO: initialized')


    def run(self):

        if not self._enabled:
            print('ANEMO: sensor has been disabled from settings'
                  '\n\tExiting thread')
            return

        print('ANEMO: cleaning port')

        # in case the port is not properly closed
        try:
            temp = serial.Serial(self._port, self._baud, serial.SEVENBITS,
                                 serial.PARITY_EVEN, serial.STOPBITS_ONE)
            temp.close()
        except:
            print('ANEMO: unable to open anemo at ' + self._port + ':'
                  + str(self._baud))
            return

        t0 = datetime.now()

        freq = self._freq
        t = datetime.now()
        t_pre = datetime.now()
        avg_number = 100.0

        print('ANEMO: opening the port')

        # open serial port and save the data
        with serial.Serial(self._port, self._baud, serial.SEVENBITS,
                           serial.PARITY_EVEN, serial.STOPBITS_ONE) as s:

            # clear buffer
            for _ in range(10):
                s.readline().decode('utf-8', 'replace')
                time.sleep(0.001)

            print('ANEMO: starting anemo main loop')

            while self._on:
                t = datetime.now()
                dt = (t - t_pre).total_seconds()
                if dt < self._desired_dt:
                    continue

                freq = (freq * (avg_number - 1) + (1.0 / dt)) / avg_number
                t_pre = t
                
                line = s.readline().decode('utf-8', 'replace')
                line = line.replace('\r', '')
                line = line.replace('\n', '')

                if len(line) < 2:
                    time.sleep(0.010)  # wait 10 millis
                    continue

                ret, uvw, temp = self.parse_line(line)
                if not ret:
                    continue
               
               
                with self._lock:
                    self._t = (t - self._t0).total_seconds()
                    self._new_data = True
                    self._freq = int(freq)
                    self._uvw = uvw
                    self._temperature = temp
                    
                self.update_data()

            print('ANEMO: thread closed')
    

    def parse_line(self, line):

        if self._mini_mode:
            return self.parse_mini_anemometer(line)
        else:
            return self.parse_big_anemometer(line)

    

    def parse_mini_anemometer(self, line):
        ret = False
        uvw = np.zeros(3)
        temp = 0.0
        
        try:
            uvw[0] = float(line[0:6])
            uvw[1] = float(line[7:13])
            uvw[2] = float(line[14:20])
            temp = float(line[21:27])
            ret = True
        except:
            ret = False
            print('ANEMO: error processing data')
            
        return ret, uvw, temp

    
    def parse_big_anemometer(self, line):
        ret = False
        uvw = np.zeros(3)
        temp = 0.0
        
        try:
            uvw[0] = float(line[0:5]) / 100.0
            uvw[1] = float(line[5:10]) / 100.0
            uvw[2] = float(line[10:15]) / 100.0
            temp = float(line[15:20]) / 100.0
            ret = True
        except:
            ret = False
            print('ANEMO: error processing data')
            
        return ret, uvw, temp


    def update_data(self):
        data = AnemoData(
            self._t,
            self._freq,
            self._uvw,
            self._temperature
        )
        rover.update_ane(self._index, data)


    def get_data(self):
        with self._lock:
            new_data = self._new_data
            self._new_data = False

            return (new_data,
                AnemoData(
                    self._t,
                    self._freq,
                    self._uvw,
                    self._temperature
                )
            )


    def end_thread(self):
        self._on = False
        print('ANEMO: thread close signal received')

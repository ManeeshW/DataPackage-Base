import serial
import numpy as np
import threading
import time

from datetime import datetime

class Anemo(threading.Thread):

    def __init__(self, thread_id, port, baud, t0, freq=10):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._t0 = t0
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

                freq = int((freq * (avg_number - 1) + (1.0 / dt)) / avg_number)
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
                    self._freq = freq
                    self._uvw = uvw
                    self._temperature = temp
    

    def parse_line(self, line):
        try:
            uvw = np.zeros(3)
            temp = 0.0

            try:
                uvw[0] = float(line[0:6])
                uvw[1] = float(line[7:13])
                uvw[2] = float(line[14:20])
                temp = float(line[21:27])
            except:
                print('ANEMO: error processing data')
            ret = True
            return ret, uvw, temp
        except:
            return False, np.zeros(3), 0.0    


    def get_data(self):
        with self._lock:
            new_data = self._new_data
            self._new_data = False

            return (new_data,
                AnemoData(
                    self._t0,
                    self._freq,
                    self._uvw,
                    self._temperature
                )
            )


    def end_thread(self):
        self._on = False


if __name__ == '__main__':
    ANEMO1_PORT = '/dev/cu.usbserial-FT1VCQ1D1'
    t0 = datetime.now()
    thread = Anemo(0, ANEMO1_PORT, 9600, t0)
    thread.start()

    while True:
        try:
            time.sleep(0.01)
            print(thread._uvw.T)
        except KeyboardInterrupt:
            thread.end_thread()
            break
    thread.join()

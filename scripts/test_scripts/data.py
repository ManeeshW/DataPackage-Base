import numpy as np
import threading

class Data:
    def __init__(self):
        self.lock = threading.Lock()

        self.on = True
        self.t = 0.0
        self.x = np.zeros(3)

    def update_x(self, x):
        with self.lock:
            self.x = x


data = Data()

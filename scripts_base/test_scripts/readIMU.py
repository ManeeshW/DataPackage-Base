import time
from time import sleep
import serial
import struct
import numpy as np
from array import array

s = serial.Serial("/dev/cu.usbserial-AK05WNDF", baudrate=115200)

def read_imu_data(s):
    # Read data.
    data = s.readline()
    print(data)
    '''
    # Check if there are unexpected errors in the message.
    checksum_array = array('B', [data[40], data[39]])
    checksum = struct.unpack('H', checksum_array)[0]

    crc = calculate_imu_crc(data[:39])

    # if not crc == checksum:
    #     print('IMU CRC error')
    #     return
    
    # TODO: endieness, only work with 'little'
    # _ = s.read(3)
    yaw = struct.unpack('f', data[3:7])[0]
    pitch = struct.unpack('f', data[7:11])[0]
    roll = struct.unpack('f', data[11:15])[0]
    W_x = struct.unpack('f', data[15:19])[0]
    W_y = struct.unpack('f', data[19:23])[0]
    W_z = struct.unpack('f', data[23:27])[0]
    a_x = struct.unpack('f', data[27:31])[0]
    a_y = struct.unpack('f', data[31:35])[0]
    a_z = struct.unpack('f', data[35:39])[0]

    print('yaw : {}\t pitch : {}\t roll : {}'.format(yaw, pitch, roll))
    '''
# Calculate the 16-bit CRC for the given message.
def calculate_imu_crc(data):
    data = bytearray(data)

    crc = np.array([0], dtype=np.ushort)
    for i in range(len(data)):
        crc[0] = (crc[0] >> 8) | (crc[0] << 8)
        crc[0] ^= data[i]
        crc[0] ^= (crc[0] & 0xff) >> 4
        crc[0] ^= crc[0] << 12
        crc[0] ^= (crc[0] & 0x00ff) << 5
    
    return crc[0]

while 1:
    #data = s.readline()
    #data = s.read(41)
    read_imu_data(s)
    #print(data)
    #s.write(rx_data)
    #data = s.read(41)
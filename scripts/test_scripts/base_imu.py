import numpy as np
import pdb
import serial
import struct

from array import array


port = '/dev/cu.usbserial-AK05WNDF'
baud = 115200

print('Reading from {} at {} Baud'.format(port, baud))

imu_sync_detected = False

def check_sync_byte(s):
    for _ in range(s.in_waiting):
        byte_in = s.read(1)
        int_in = int.from_bytes(byte_in, 'little')
        if int_in == 250:
            return True


def read_imu_data(s):
    # Read data.
    data = s.read(41)

    # Check if there are unexpected errors in the message.
    checksum_array = array('B', [data[40], data[39]])
    checksum = struct.unpack('H', checksum_array)[0]

    crc = calculate_imu_crc(data[:39])

    if not crc == checksum:
        print('IMU CRC error')
        return
    
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

    print('{}\t{}\t{}'.format(yaw, pitch, roll))


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


with serial.Serial(port, baud, timeout=1) as s:

    # Clear the buffer first.
    print('Clearing buffer ..')
    num_bytes = s.in_waiting
    s.read(num_bytes)

    print('Strating main loop')
    while True:
        imu_sync_detected = False

        try:
            num_bytes = s.in_waiting
            if num_bytes == 0:
                continue

            imu_sync_detected = check_sync_byte(s)
            if not imu_sync_detected:
                continue

            read_imu_data(s)

        except KeyboardInterrupt:
            print('Ending program ..')
            break

print('Program closed')

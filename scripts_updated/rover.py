import time
import socket
from datetime import datetime

# from common_types import ImuData, GpsData
# from imu import ImuBno0555
from gps import SwiftnavPiksiMulti
from anemo import Anemo
from log import Log
from wifi import WifiServer
from camera import LogitecCamera
from wifi_server_thread1 import ServerThread1
from wifi_server_thread2 import ServerThread2
from imu2 import VectornavVn100

res = "\033[0m"
BL = "\033[5m"
M = "\033[35m"
R = "\033[31m"
G = "\033[32m"

Client1_ON = True  
Client2_ON = False

logfreq = 50
GPS_ON = True
IMU_ON = True
CAM1_ON = False
CAM2_ON = False


ANE1_ON = False
ANE2_ON = False
MINI_ANEMO_MODE=True
# These won't change, unless there is a hardware change.
GPS_PORT = '/dev/ttyUSB0' #dev/ttyAMA1
IMU_PORT = '/dev/ttyTHS1' #dev/ttyAMA2
ANEMO1_PORT = '/dev/ttyUSB0'
ANEMO2_PORT = '/dev/ttyUSB1'

SERVER = '192.168.8.1'
CLIENT1 = '192.168.8.98'
#Ship Wifi Extender-192.168.8.24 myExtender 192.168.8.20 rpi-'192.168.8.23' #192.168.8.24
CLIENT2 = '192.168.8.22' # 192.168.8.22 for 'rover' panda wireless dongle (device specific static IP)
PORT = 5000



def main():
    
    ServerSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        ServerSocket.bind((SERVER, PORT))
        print('\033[97m'+'\033[40m'+"Wifi Server started ..."+'\033[49'+'\033[0m')
    except socket.error as e:
        print(str(e))
    i = 1
    while True:
        print(M+"Trying to connect client {:d} ...".format(i)+res)
        ServerSocket.listen(1)
        conn, addr = ServerSocket.accept()
        print("Client 1 IPs : ",addr )
        if Client1_ON:
            if addr[0] == CLIENT1:
                print(G+"Client 1"+res, CLIENT1 ,G+"successfully connected ..."+res)
                client1thread = ServerThread1(6, conn, addr)
                i = 2
            #else:
                #print("Assume Client 1 connected to wifi extender ...\n IP :", addr[0] ,"Client 1 successfully connected ...")
                #client1thread = ServerThread1(6, conn, addr)
                #i = 2
        print(M+"tying to connect Client 2 ip ",addr,res)
        if Client2_ON:
            if addr[0] == CLIENT2:
                print(G+"Client 2"+res,CLIENT2,G+" successfully connected ...")
                client2thread = ServerThread2(7, conn, addr)
                break
        else:
            break

    print('MAIN: Starting rover'+res)

    t0 = datetime.now()

    #thread_imu = ImuBno0555(1, t0, IMU_ON)
    thread_imu = VectornavVn100(1, IMU_PORT, 230400, t0, IMU_ON)
    thread_gps = SwiftnavPiksiMulti(2, GPS_PORT, 115200, t0, GPS_ON)
    thread_ane1 = Anemo(3, ANEMO1_PORT, 9600, 1, t0, ANE1_ON, MINI_ANEMO_MODE)
    thread_ane2 = Anemo(4, ANEMO2_PORT, 9600, 2, t0, ANE2_ON, MINI_ANEMO_MODE)
    thread_log = Log(5, t0, logfreq)
    #thread_cam1 = LogitecCamera(5, t0, CAM1_ON)
    #thread_wifi = WifiServer(7)

    
    threads = []
    threads.append(thread_imu)
    threads.append(thread_gps)
    threads.append(thread_ane1)
    threads.append(thread_ane2)
    #threads.append(thread_cam1)
    threads.append(thread_log)
    #threads.append(thread_wifi)
    
    if Client1_ON:
        threads.append(client1thread)
    if Client2_ON:
        threads.append(client2thread)

    for thread in threads:
        thread.start()

    exit_flag = False

    while not exit_flag:
        try:
            time.sleep(0.01)

            if not client1thread.is_system_on(): #*************************************************
            #if not thread_wifi.is_system_on():
                exit_flag = True

        except KeyboardInterrupt:
            exit_flag = True
            print('\nMAIN: Stopping rover')

    
    print('MAIN: Sending close signal to threads')
    for thread in threads:
        if thread._enabled:
            thread.end_thread()
            thread.join()

    print('MAIN: Rover closed!')


if __name__ == '__main__':
    main()

import socket

import proto.data_pb2

data = proto.data_pb2.Data()
data.t = 0
data.t_system = 2020111111.0
data.ypr.x = 0.0
data.ypr.y = 0.0
data.ypr.z = 0.0
data_str = data.SerializeToString()
print(data_str)

HOST = 'localhost'  # The server's hostname or IP address
PORT = 9999        # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(data_str)
    data = s.recv(1024)

data_new = proto.data_pb2.Data()
data_new.ParseFromString(data)
print('Received', data_new)


# print(data_new)

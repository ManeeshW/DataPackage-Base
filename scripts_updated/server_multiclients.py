import socket
import os
from _thread import *

ServerSocket = socket.socket()
host = '10.42.0.1'
port = 5000
ThreadCount = 0
try:
    ServerSocket.bind((host, port))
except socket.error as e:
    print(str(e))

print('Waitiing for a Connection..')
ServerSocket.listen(5)


def threaded_client(connection):
    connection.send(str.encode('Welcome to the Server'))
    while True:
        data = connection.recv(2048)
        reply = 'Server Says: ' + data.decode('utf-8')
        if not data:
            break
        connection.sendall(str.encode(reply))
    connection.close()

def threaded_client2(connection):
    connection.send(str.encode('Welcome to the Server'))
    while True:
        data = connection.recv(2048)
        reply = 'Server Says: ' + data.decode('utf-8')
        if not data:
            break
        print("cl2")
        connection.send(str.encode('Welcome to the Server'))
        connection.sendall(str.encode(reply))
    connection.close()

while True:
    Client, address = ServerSocket.accept()
    print("Client : ", Client)
    print('Connected to: ' + address[0] + ':' + str(address[1]))
    if address[0] == "10.42.0.172": #nano
        start_new_thread(threaded_client2, (Client, ))
        ThreadCount += 1
    else:
        start_new_thread(threaded_client, (Client, ))
        ThreadCount += 1

    print('Thread Number: ' + str(ThreadCount))
ServerSocket.close()
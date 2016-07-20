# coding: utf-8
import socket

# LOCAL_HOST = "194.167.1.1"
LOCAL_HOST = "127.0.0.1"
LOCAL_PORT = 8000
MAX_BUFFER_SIZE = 5000
MAX_CONN = 1

def main():
    socket_server = socket.socket(socket.AF_INET)
    socket_server.bind((LOCAL_HOST, LOCAL_PORT))
    socket_server.listen(MAX_CONN)
    print 'Listening', LOCAL_HOST, LOCAL_PORT
    print 'Waiting for clients connections.'

    conn, client_addr  = socket_server.accept()
    print 'Connected to ', client_addr
    print 'Will wait for data...'

    while True:
        msg = conn.recv(MAX_BUFFER_SIZE)
        if msg:
            print 'Received: ', msg
        if msg == "quit":
            print 'Client left, I will leave to'
            break

    print 'Bye.'

if __name__ == '__main__':
    main()

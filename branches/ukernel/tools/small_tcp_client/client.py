# coding: utf-8
import socket
import sys
import array
import time

PILSENER = "194.167.1.151" # c2.a7.1.97 (pilsener)
DUNKEL = "127.0.0.1" # dunkel local host
global REMOTE_HOST

REMOTE_PORT = 8000
MAX_BUFFER_SIZE = 1024
MAX_CONN = 1
PDU = 500
ITERATIONS = 3


def fixed(socket_client): # Same behavior as EPOS ping.cc application (tcp_test).

    data = array.array('c', ['0'] * PDU)

    for i in range(ITERATIONS):
        data[0] = '\n'
        data[1] = ' '
        data[2] = chr(ord('0') + i)
        data[3] = chr(ord('0') + i)
        data[4] = chr(ord('0') + i)
        data[5] = chr(ord('0') + i)
        data[6] = chr(ord('0') + i)
        data[7] = chr(ord('0') + i)

        j = 8
        while j < len(data) - 8:
            data[j+0] = ' '
            data[j+1] = chr(ord('0') + i + (j / 1000000 % 10))
            data[j+2] = chr(ord('0') + (j / 100000 % 10))
            data[j+3] = chr(ord('0') + (j / 10000 % 10))
            data[j+4] = chr(ord('0') + (j / 1000 % 10))
            data[j+5] = chr(ord('0') + (j / 100 % 10))
            data[j+6] = chr(ord('0') + (j / 10 % 10))
            data[j+7] = chr(ord('0') + (j % 10))
            j += 8

        data[len(data) - 8] = ' '
        data[len(data) - 7] = chr(ord('0') + i)
        data[len(data) - 6] = chr(ord('0') + i)
        data[len(data) - 5] = chr(ord('0') + i)
        data[len(data) - 4] = chr(ord('0') + i)
        data[len(data) - 3] = chr(ord('0') + i)
        data[len(data) - 2] = '\n'
        data[len(data) - 1] = '\0'

        time.sleep(1)
        sent = socket_client.send(data.tostring())
        if sent == len(data):
            print "  Data: ", data.tostring()
        else:
            print "  Data was not correctly sent. It was ", len(data), " bytes long, but only ", sent, "bytes were sent!"


    if REMOTE_HOST == DUNKEL:
        socket_client.send("quit")



def interactive(socket_client):
    print 'Connected. Type data to be send.'

    while True:
        msg = str(raw_input(">"))
        socket_client.send(msg)
        if msg == "quit":
            break
        print "sent: " + msg


def main(argv):
    global REMOTE_HOST
    REMOTE_HOST = DUNKEL

    if '-pilsener' in argv:
        REMOTE_HOST = PILSENER

    socket_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    print 'Trying to connect to', REMOTE_HOST, REMOTE_PORT
    socket_client.connect((REMOTE_HOST,REMOTE_PORT))

    if '-fixed' in argv:
        fixed(socket_client)
    else:
        interactive(socket_client)

    socket_client.close()

    print 'Bye.'

if __name__ == '__main__':
    main(sys.argv)

# coding: utf-8
import socket
import sys
import array
import time

E100_PILSENER = '\x00\x90\x27\x9a\x3b\x97'
E100_DUNKEL = '\x00\x90\x27\x57\x1F\xF1'
ETHER_TYPE = '\x88\x88'
HEADER_SIZE = 14

PDU = 1500

def fixed(socket_client, time_between_frames, iterations): # Same behavior as EPOS ping.cc application (tcp_test).
    print('Using TBF = ' + str(time_between_frames))
    print('Iterations = ' + str(iterations))
    assert iterations

    src = E100_DUNKEL
    dst = E100_PILSENER
    ether_type = ETHER_TYPE

    data = array.array('c', ['0'] * PDU)

    itera = 0
    if iterations == 'INFINITE':
        shall_iterate = True
    else:
        shall_iterate = itera < iterations

    i = 0
    while shall_iterate:
        if i > 255 - ord('0'):
            i = 0

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

        time.sleep(time_between_frames)
        sent = socket_client.send(dst + src + ether_type + data.tostring())

        #if sent == len(data):
        #    print("  Data: ", data.tostring())
        #else:
        #    print("  Data was not correctly sent. It was ", len(data), " bytes long, but only ", sent, "bytes were sent!")
        if sent != len(data) + HEADER_SIZE:
            print("  Data was not correctly sent. It was ", len(data), " bytes long, but only ", sent, "bytes were sent!")

        i += 1

        if iterations == 'INFINITE':
            shall_iterate = True
        else:
            itera += 1
            shall_iterate = itera < iterations


def main(argv):
    time_between_segments = 1.0
    iterations = 0

    print('Ethernet client')
    print('Client argv:')
    print(argv)

    if '-DONTRUN' in argv:
        print('Will not run the Ethernet client')
        print('Bye!')
        return

    if '-iterations' not in argv:
        print('You should inform iterations')
        print('exiting...')
        return
    else:
        i = argv[argv.index('-iterations') + 2]
        if i != 'INFINITE':
            iterations = int(i)
        else:
            iterations = i

    if '-tbs' in argv:
        time_between_segments = float(argv[argv.index('-tbs') + 2])
    else:
        print('tbs not informed. Default to = ' + str(time_between_segments))

    socket_client = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)

    interface = 'eth1'
    socket_client.bind((interface, 0))
    print('Binded to interface: ' + str(interface))


    fixed(socket_client, time_between_segments, iterations)


    socket_client.close()

    print('Bye.')

if __name__ == '__main__':
    main(sys.argv)

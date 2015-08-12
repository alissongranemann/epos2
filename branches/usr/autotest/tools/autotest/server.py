'''
Created on 29/05/2015

@author: andre
'''
import threading
import socket
import datetime

FILES_PATH = '.'  # '~/Documents/epos_log_files'
HOST = ''  # Symbolic name meaning all available interfaces
PORT = 50000  # Arbitrary non-privileged port
BUFF_SIZE = 1500
SINGLE = 'S'
MULTIPLE = 'M'

def create_file(user):
    ct = datetime.datetime.now()
    time = '%s-%s-%s %s:%s:%s' % (ct.year, ct.month, ct.day, ct.hour, ct.minute, ct.second)
    file_name = user + ' ' + time
    fd = open('%s/%s %s' % (FILES_PATH, file_name), 'w')
    return fd, file_name

def handle_connection(connection, address):
        print 'Connection accepted from', address
        fd, file_name = create_file(address[0])
        while True:
            data = connection.recv(BUFF_SIZE)
            if data == '@$end$@': break
            print 'Received from {}.:'.format(address), data
            fd.write(data)
        msg = 'Data Writen to file %s at %s' % (file_name, FILES_PATH)
        print msg
        connection.send(msg)
        fd.close()
        print 'closing connection to', address
        connection.close()
    
def main():
    connection_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection_socket.bind((HOST, PORT))
    connection_socket.listen(4)
    while True:
        threading.Thread(target=handle_connection, args=connection_socket.accept())
    connection_socket.close()
    pass

if __name__ == '__main__':
    main()
    pass

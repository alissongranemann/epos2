import serial
import struct
import requests
import traceback
import sys
import time
import json

from gateway_daemon.mote import Mote
from gateway_daemon.modbus import parse


MODBUS_FUNCTIONS = {
    3 : 'numeric',
    1 : 'binary'
}
def clean_data(data):
    '''Remove /r/n'''
    while len(data) > 0 and (data[-1] == '\r' or data[-1] == '\n'):
        data = data[:-1]
    return data


def free_data(data):
    try:
        register, cmd, bytes = parse(data)
        command = MODBUS_FUNCTIONS[cmd]
        #print(register, command, bytes, data)
    except ValueError:
        raise
    except KeyError:
        pass
    try:
        offset = int.from_bytes(bytes[:2], byteorder='big')
        value = int.from_bytes(bytes[2:], byteorder='big')
    except struct.error:
        raise
    return (hex(register).split('x')[1].upper(), command, str(offset),
str(value))


def mount_request(data):
    try:
        register, command, offset, value = free_data(data)
        #print(register, command, offset, value)
        post=[]
        post_this={register+command+offset:value}
        post_kairos={ "name": str(register+command+offset), "value": float(value), "timestamp": int(time.time()*1000), "tags": { "project": "smarthome" }}
        post.append(post_this)
        post.append(post_kairos)
        return post
    except KeyError:
        raise
    except ValueError:
        raise
    except struct.error:
        raise

def make_request(data):
    data = clean_data(data)
    try:
        post = mount_request(data)
        to_post = post[0]
        to_post_kairos = post[1]
        #done_post = requests.post('http://sv13.lisha.ufsc.br:8080/ScadaBR/httpds', params = to_post)
        #done_post = requests.post('http://localhost:8080/ScadaBR/httpds', params = to_post)
        #done_post = requests.post('http://150.162.142.42:8080/ScadaBR/httpds', params = to_post)
        #done_post = requests.post('http://150.162.62.12:8080/ScadaBR/httpds', params = to_post)
        done_post = requests.post('http://ap2.lisha.ufsc.br:8080/ScadaBR/httpds', params = to_post)
        done_kairos_lehder = requests.post('http://iot.lisha.ufsc.br:5000/convert/', data = json.dumps(to_post_kairos))
    except (KeyboardInterrupt, SystemExit):
        raise
    except:
        print("Error in message", data, file=sys.stderr)
        traceback.print_exc()
    #except KeyError:
    #    pass
    #except ValueError:
    #    pass
    #except requests.exceptions.ConnectTimeout:
    #    pass
    #except struct.error:
    #    pass

def run_serial():
    serial_connection = serial.Serial('/dev/ttyS2', 115200, timeout=None)
    mote = Mote(serial_connection)
    while True:
        data = mote.readline()
        print("Read from serial:", data)
        make_request(data)

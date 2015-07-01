import serial
import sys
import time
import os.path
import struct

handshake1_message = b'h\x00\x00\x00\x00\x00\x00\x00\x00'
handshake2_message = b'H\x00\x00\x00\x00\x00\x00\x00\x00'
end_message = b'e\x00\x00\x00\x00\x00\x00\x00\x00'    

hex_offset = 0
sequence_number = 0
last_address = 0 # This script assumes increasing addresses. It will ignore addresses out of increasing order.

def hex_to_msg(line):
    def checksum(line):
        acc = 0
        for i in range(0, len(line)-1, 2):
            acc += int(line[i:i+2], base=16)
        acc = acc & 0xff
        return abs(acc - 2**8) & 0xff

    def str_to_bytes(string):
        def split_by(seq, n):
            for i in range(0, len(seq), n):
                yield seq[i:i+n]
        return bytes(int(x, 16) for x in split_by(string, 2))

    global hex_offset
    global sequence_number
    global last_address

    #print(line)

    if line[-1:] == '\n':
        line = line[:-1]

    start_code = line[0]
    if start_code != ':':
        raise Exception("Start code is not ':'", line)
    c = checksum(line[1 : -2])
    c0 = int(line[-2 : ], base=16)
    if c != c0:
        raise Exception("Wrong checksum", line, hex(c0), hex(c))

    ret = []

    byte_count = int(line[1:3], base=16)
    address = int(line[3:7], base=16) + hex_offset
    record_type = int(line[7:9], base=16)
    data = line[9 : 9+(2*byte_count)]
    #print(hex(byte_count), hex(address), hex(record_type), data, hex(hex_offset))

    if record_type == 0x00: # Data
        #print("Full data: ", data)
        for i in range(0, byte_count*2 - 7, 8):
            if address > last_address:
                msg = struct.pack('<H', sequence_number) # unsigned little-endian short
                sequence_number += 1
                msg += b'w' # write message
                print(" Address: ", hex(address), end='\r')
                d = struct.pack('>H',int(data[i:i+4], 16))
                d += struct.pack('>H',int(data[i+4:i+8], 16))
                #print("Data: ", data[i:i+8])
                msg += struct.pack('<I',address) # unsigned little-endian integer
                msg += d
                #msg += struct.pack('<I',int(data[i:i+8], 16)) # unsigned little-endian integer
                ret.append(msg)
                last_address = address
            address += 4

        i+=8 # in Python, the variable holds the last valid value of the loop

        if (i < byte_count*2) and (byte_count*2 - i < 4):
            if address > last_address:
                msg = struct.pack('<H', sequence_number) # unsigned little-endian short
                sequence_number += 1
                msg += b'w' # write message
                print(" Address: ", hex(address), end='\r')
                d = struct.pack('>H',int(data[i:], 16))
                d += struct.pack('>H', 0)
                #print("Data: ", d)
                msg += struct.pack('<I',address) # unsigned little-endian integer
                msg += d
                #msg += struct.pack('<I',int(data[i:i+8], 16)) # unsigned little-endian integer
                ret.append(msg)
                last_address = address
            address += 4

        elif i < byte_count*2:
            if address > last_address:
                msg = struct.pack('<H', sequence_number) # unsigned little-endian short
                sequence_number += 1
                msg += b'w' # write message
                print(" Address: ", hex(address), end='\r')
                d = struct.pack('>H',int(data[i:i+4], 16))
                d += struct.pack('>H',int(data[i+4:], 16))
                #print("Data: ", d)
                msg += struct.pack('<I',address) # unsigned little-endian integer
                msg += d
                #msg += struct.pack('<I',int(data[i:i+8], 16)) # unsigned little-endian integer
                ret.append(msg)
                last_address = address
            address += 4
            
        return ret

    elif record_type == 0x01: # End Of file
        msg = struct.pack('<H', sequence_number)
        sequence_number += 1
        msg += end_message
        ret.append(msg)
        return ret

    elif record_type == 0x02: # Extended Segment Address
        pass

    elif record_type == 0x03: # Start Segment Address
        pass

    elif record_type == 0x04: # Extended Linear Address        
        hex_offset = int(data, 16) << 16

    elif record_type == 0x05: # Start Linear Address
        pass

    return []


if len(sys.argv) < 2:
    print("usage:" , sys.argv[0] , "<device>")
    sys.exit(1)


dev = sys.argv[1]

print("Waiting for", dev, "to appear")
while not os.path.exists(dev):
    pass

print(dev, "found, trying to open it")

mote = serial.Serial(baudrate = 115200, port=dev, timeout=1)
mote.close()
mote.open()

print(dev, "Opened. Sending handshake messages for 1 second")
t1 = time.time()
while True:
    msg = struct.pack('<H', sequence_number) + handshake1_message
    mote.write(msg)
    t2 = time.time()
    if t2-t1 >= 1:
        break
    time.sleep(0.5)

sequence_number += 1

print("===========================================")
print("This is what the mote said in the meantime:\n")
handshake = False
k = mote.read(mote.inWaiting())
msg = struct.pack('<H', sequence_number) + handshake2_message
if msg in k:
    handshake = True
print(k.decode(), end='')
print("\n===========================================")

sequence_number += 1

if handshake:
    print("Handshake response detected!")
else:
    print("Handshake not detected. Exiting.")
    mote.close()
    sys.exit(1)

with open(sys.argv[2], 'r') as hex_file:
    for line in hex_file:
        ret = hex_to_msg(line)
        for msg in ret:
            #print('\n',msg)
            mote.write(msg)
    #        mote.flush()
    #        time.sleep(1.002)
            time.sleep(0.002)
    #        k = mote.read(mote.inWaiting())
    #        print(k)
    print('\nDone!')

sys.exit(0)

for seq in range(2,256):
    msg = struct.pack('b',seq) # byte
    msg += b'w'
    address = 0x00207654
    data = 0xdeadbbbb
    msg += struct.pack('<I',address) # little-endian integer
    msg += struct.pack('<I',data) # little-endian integer
    print("Sending write message")
    print(msg)
    mote.write(msg)

    time.sleep(2)
    k = mote.read(mote.inWaiting())
    print("Response: ", k)
    time.sleep(2)

    print("Rebooting the mote, closing it and exiting")
    msg = struct.pack('b',seq+1) # byte
    mote.write(msg + end_message)
    k = mote.read(mote.inWaiting())
    print("Response: ", k)
    mote.close()
    break

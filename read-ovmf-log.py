#!/usr/bin/env python3
import re
import struct
import sys

def repl_hex(m):
    the_bytes = m[1]
    hex_val = 'HEXDECODED=0x{:016x}'.format(struct.unpack('q', the_bytes)[0])
    if the_bytes.isalnum():
        hex_val += ' ASCII=' + the_bytes.decode('ascii')
    return hex_val.encode('utf-8')

def process(filename):
    try:
        with open(filename, 'rb') as f:
            buf = f.read()
            new_buf = re.sub(b'HEX\|(........)\|', repl_hex, buf)
            sys.stdout.buffer.write(new_buf)
            #print(new_buf.decode("utf-8"))
    except BrokenPipeError:
        pass

if len(sys.argv) > 1:
    process(sys.argv[1])
else:
    process('ovmf.log')

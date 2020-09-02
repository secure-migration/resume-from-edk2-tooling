#!/usr/bin/env python3
import re
import struct
import sys

def is_all_printable(buf):
    for b in buf:
        if b < 32 or b > 127:
            return False
    return True

def repl_hex(m):
    the_bytes = m[1]
    hex_val = 'HEXDECODED=0x{:016x}'.format(struct.unpack('Q', the_bytes)[0])
    if is_all_printable(the_bytes):
        hex_val += ' ASCII=|{}|'.format(the_bytes.decode('ascii'))
    return hex_val.encode('utf-8')

def process(filename):
    try:
        with open(filename, 'rb') as f:
            buf = f.read()
            new_buf = re.sub(b'HEX\|(........)\|', repl_hex, buf, flags=re.DOTALL)
            sys.stdout.buffer.write(new_buf)
    except BrokenPipeError:
        pass

if len(sys.argv) > 1:
    process(sys.argv[1])
else:
    process('ovmf-2.log')

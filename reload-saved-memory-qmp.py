#!/usr/bin/env python3

import sys
import os
import time

sys.path.append('/home/dmurik/qemu/python')
from qemu.qmp import QEMUMonitorProtocol

KB = 1024
MB = 1024 * KB
GB = 1024 * MB

target_socket = sys.argv[1]

print("--- Connecing to {}".format(target_socket))
srv = QEMUMonitorProtocol(target_socket)
srv.connect()

print("--- stop")
rsp = srv.cmd("stop")
print(rsp)

print("--- load part 1")
rsp = srv.cmd("pmemload", {"val": 0x0, "size": 0x80b000, "filename": "mem-dump-1.bin"})
print(rsp)
print("--- load part 2")
rsp = srv.cmd("pmemload", {"val": 0x80e000, "size": 0x3F7F2000, "filename": "mem-dump-2.bin"})
print(rsp)

#print("--- load part 3 hpet memory")
#rsp = srv.cmd("pmemload", {"val": 0xfed00000, "size": 1 * KB, "filename": "hpet-mem-dump.bin"})
#print(rsp)

print("--- load devices state")
rsp = srv.cmd("xen-load-devices-state", {"filename": "devices-state.bin"})
print(rsp)

print("--- load flag")
rsp = srv.cmd("pmemload", {"val": 0x80df08, "size": 0x8, "filename": "mem-flag.bin"})
print(rsp)

time.sleep(0.5)

print("--- cont")
rsp = srv.cmd("cont")
print(rsp)

print("--- done")
srv.close()

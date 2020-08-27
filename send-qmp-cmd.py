#!/usr/bin/env python3

import sys
import os
import time

sys.path.append('/home/dmurik/qemu/python')
from qemu.qmp import QEMUMonitorProtocol

target_socket = sys.argv[1]
user_cmd = sys.argv[2]

print("--- Connecing to {}".format(target_socket))
srv = QEMUMonitorProtocol(target_socket)
srv.connect()

print("--- {}".format(user_cmd))
rsp = srv.cmd(user_cmd)
print(rsp)
srv.close()

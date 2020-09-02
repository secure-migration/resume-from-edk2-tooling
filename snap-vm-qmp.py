#!/usr/bin/env python3

import sys
import os
import re
import time

sys.path.append('/home/dmurik/qemu/python')
from qemu.qmp import QEMUMonitorProtocol

KB = 1024
MB = 1024 * KB
GB = 1024 * MB

def hmp_cmd(srv, command_line):
    print(">>> HMP: {}".format(command_line))
    rsp = srv.cmd("human-monitor-command", {"command-line": command_line})
    print(rsp['return'])
    return rsp['return']

def shell_cmd(command_line):
    print(">>> Shell: {}".format(command_line))
    os.system(command_line)

target_socket = sys.argv[1]

print("--- Connecing to {}".format(target_socket))
srv = QEMUMonitorProtocol(target_socket)
srv.connect()

print("--- stopping")
rsp = srv.cmd("stop")
print(rsp)

info_registers = hmp_cmd(srv, "info registers")
hmp_cmd(srv, "info lapic")
hmp_cmd(srv, "info pic")
hmp_cmd(srv, "gva2gpa 0xfffffe0000000000")
hmp_cmd(srv, "gva2gpa 0xfffffe0000001000")
hmp_cmd(srv, "gva2gpa 0xfffffe0000003000")
hmp_cmd(srv, "x /32xg 0xfffffe0000000000")
hmp_cmd(srv, "x /16xg 0xfffffe0000001000")
hmp_cmd(srv, "x /32xg 0xfffffe0000003000")

hmp_cmd(srv, "savevm save3")

print("--- dump memory")
rsp = srv.cmd("pmemsave", {"val": 0x0, "size": 1 * GB, "filename": "mem-dump.bin"})
print(rsp)

print("--- exiting")
rsp = srv.cmd("quit", {})
print(rsp)

srv.close()

# The value of CR3 from the source VM
m = re.search("CR3=([0-9a-f]+) ", info_registers)
cr3 = "0x{}".format(m[1])

# The virtual address of our RestoreRegisters page in the linux kernel offset
# mapping
virt_addr = "0xffff88800080c000"

print("--- clear NX flag")
shell_cmd("./memdump/modify-mem-dump mem-dump.bin {} {}".format(cr3, virt_addr))

print("--- save mem-dump-1.bin")
shell_cmd("dd if=mem-dump.bin of=mem-dump-1.bin bs=4096 count={}".format(0x80b))
print("--- save mem-dump-2.bin")
shell_cmd("dd if=mem-dump.bin of=mem-dump-2.bin bs=4096 skip={} count={}".format(0x80e, 0x3f7f2))
print("--- done")

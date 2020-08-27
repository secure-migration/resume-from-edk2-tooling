#!/usr/bin/env python3

import re
import sys

s = {}

for line in sys.stdin:
    p = re.findall(r'([A-Z0-9_]+ ?)=([0-9a-f]+)', line)
    for m in p:
        reg = m[0].strip().lower()
        val = m[1].lower()
        s[reg] = val
    m = re.match(r'^(.DT)=\s*([0-9a-f]+)\s+([0-9a-f]+)', line)
    if m is not None:
        xdt = m[1].lower()
        s[xdt + '_addr'] = m[2]
        s[xdt + '_size'] = m[3]
    m = re.match(r'^(FS|GS) =[0-9a-f]+\s*([0-9a-f]+)', line)
    if m is not None:
        seg = m[1].lower()
        s[seg + '_base'] = m[2]


code = """
  s.regs.ax = 0x{rax};
  s.regs.bx = 0x{rbx};
  s.regs.cx = 0x{rcx};
  s.regs.dx = 0x{rdx};
  s.regs.si = 0x{rsi};
  s.regs.di = 0x{rdi};
  s.regs.bp = 0x{rbp};
  s.regs.sp = 0x{rsp};
  s.regs.r8 = 0x{r8};
  s.regs.r9 = 0x{r9};
  s.regs.r10 = 0x{r10};
  s.regs.r11 = 0x{r11};
  s.regs.r12 = 0x{r12};
  s.regs.r13 = 0x{r13};
  s.regs.r14 = 0x{r14};
  s.regs.r15 = 0x{r15};
  s.regs.flags = 0x{rfl};
  s.regs.orig_ax = 0x{rax};
  s.regs.ip = 0x{rip};
  s.regs.cs = 0x{cs};
  s.regs.ss = 0x{ss};
  s.ds = 0x{ds};
  s.es = 0x{es};
  s.fs = 0x{fs};
  s.gs = 0x{gs};
  s.tr = 0x{tr};
  s.fs_base = 0x{fs_base};
  s.gs_base = 0x{gs_base};
  s.kernelgs_base = 0x{msr_kernelgsbase};
  s.cr0 = 0x{cr0};
  s.cr2 = 0x{cr2};
  s.cr3 = 0x{cr3};
  s.cr4 = 0x{cr4};
  s.efer = 0x{efer};
  s.sysenter_cs = 0x{msr_sysenter_cs};
  s.sysenter_esp = 0x{msr_sysenter_esp};
  s.sysenter_eip = 0x{msr_sysenter_eip};
  s.star = 0x{msr_star};
  s.lstar = 0x{msr_lstar};
  s.cstar = 0x{msr_cstar};
  s.fmask = 0x{msr_fmask};
  s.tsc_aux = 0x{msr_tsc_aux};
  s.gdt_desc.size = 0x{gdt_size};
  s.gdt_desc.address = 0x{gdt_addr};
  s.idt.size = 0x{idt_size};
  s.idt.address = 0x{idt_addr};
"""
print(code.format_map(s));
print("/*");
print(s);
print("*/");

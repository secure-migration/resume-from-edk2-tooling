#!/bin/bash

# map CTRL-C to CTRL ]
echo "Mapping CTRL-C to CTRL-]"
stty intr ^]

echo "Launching VM ..."

# sudo qemu-system-x86_64 \

#sudo gdb -ex 'handle SIGUSR1 noprint' \
#	--args /home/dmurik/qemu/x86_64-softmmu/qemu-system-x86_64 \
sudo /home/dmurik/qemu/x86_64-softmmu/qemu-system-x86_64 \
	-name dubek2 \
	-enable-kvm -cpu EPYC -machine q35 -m 1G \
	-drive if=pflash,format=raw,unit=0,file=/home/tobin/trampoline/edk2-internal/Build/AmdSev/DEBUG_GCC5/FV/AMDSEV.fd,readonly \
	-fw_cfg name=opt/ovmf/PcdSevIsMigrationHandler,string=1 \
	-fw_cfg name=opt/ovmf/PcdMigrationStateCR3,string=0x3a71c000 \
	-fw_cfg name=opt/ovmf/PcdMigrationStatePage,file=/home/tobin/trampoline/state-blob/cpu_state.bin \
	-drive file=/home/tobin/trampoline/ubuntu-mod.img,if=none,id=disk0,format=qcow2 \
	-device virtio-scsi-pci,id=scsi,disable-legacy=on,iommu_platform=true \
	-device scsi-hd,drive=disk0 \
    -display vnc=:32 \
	-global isa-debugcon.iobase=0x402 -debugcon file:ovmf.log \
	-qmp unix:/home/tobin/trampoline/qmp-sock,server,nowait \
    -chardev stdio,id=char0,logfile=serial.log,signal=off,mux=on \
    -serial chardev:char0 \
	-monitor unix:/home/tobin/trampoline/qemu-sock,server,nowait \
    -monitor chardev:char0 \
	-loadvm save3 2> qemu-stderr.log

# restore the mapping
# stty intr ^c
# echo "VM stopped. Restored CTRL-C behaviour"

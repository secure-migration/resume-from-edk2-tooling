#!/bin/bash

NET_ARGS="-nic user,mac=52:54:00:66:77:88,hostfwd=tcp::9022-:22"
if [ x"$1" = x"-nonet" ] ; then
  NET_ARGS=""
fi

# map CTRL-C to CTRL ]
echo "Mapping CTRL-C to CTRL-]"
stty intr ^]

echo "Launching VM ..."

# sudo qemu-system-x86_64 \

sudo /home/dmurik/qemu/x86_64-softmmu/qemu-system-x86_64 \
	-name dubek2 \
	-enable-kvm -cpu EPYC -machine q35 -m 1G \
	-drive if=pflash,format=raw,unit=0,file=$HOME/trampoline/edk2-internal/Build/AmdSev/DEBUG_GCC5/FV/AMDSEV.fd,readonly \
	-fw_cfg name=opt/ovmf/PcdSevIsMigrationHandler,string=0 \
	-drive file=$HOME/trampoline/ubuntu-mod.img,if=none,id=disk0,format=qcow2 \
	-device virtio-scsi-pci,id=scsi,disable-legacy=on,iommu_platform=true \
	-device scsi-hd,drive=disk0 \
	$NET_ARGS \
    -nographic \
	-global isa-debugcon.iobase=0x402 -debugcon file:ovmf.log \
	-qmp unix:$HOME/trampoline/qmp-sock,server,nowait \
	-monitor unix:$HOME/trampoline/qemu-sock,server,nowait \
        -monitor pty

	#-drive file=tmp.qcow2,if=none,id=disk0,format=qcow2 \
	#-drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE.fd,readonly \
#
	#-global isa-debugcon.iobase=0x402 -debugcon file:ovmf.log \

	#-chardev stdio,id=char0,logfile=serial.log,signal=off,mux=on \
	#-serial chardev:char0 \
	#-mon chardev=char0 \

# restore the mapping
stty intr ^c
echo "VM stopped. Restored CTRL-C behaviour"

exit 0


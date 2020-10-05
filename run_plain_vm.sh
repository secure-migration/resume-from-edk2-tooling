#!/bin/bash

SSHPORT=${VM_SSH_PORT:-9022}
NET_ARGS="-nic user,mac=52:54:00:66:77:88,hostfwd=tcp::${SSHPORT}-:22"
if [ x"$1" = x"-nonet" ] ; then
  NET_ARGS=""
fi

# map CTRL-C to CTRL ]
echo "Mapping CTRL-C to CTRL-]"
stty intr ^]

echo "Launching VM ..."

sudo /home/dmurik/qemu/build/x86_64-softmmu/qemu-system-x86_64 \
	-name "vm-$USER" \
	-enable-kvm -cpu EPYC -machine q35 -m 1G \
	-drive if=pflash,format=raw,unit=0,file=$HOME/trampoline/edk2-internal/Build/AmdSev/DEBUG_GCC5/FV/AMDSEV.fd,readonly \
	-fw_cfg name=opt/ovmf/PcdSevIsMigrationHandler,string=0 \
	-drive file=$HOME/trampoline/ubuntu-mod.img,if=none,id=disk0,format=qcow2 \
	-device virtio-scsi-pci,id=scsi,disable-legacy=on,iommu_platform=true \
	-device scsi-hd,drive=disk0 \
	$NET_ARGS \
    -nographic \
	-global isa-debugcon.iobase=0x402 -debugcon file:ovmf-1.log \
	-qmp unix:$HOME/trampoline/qmp-sock,server,nowait \
    -chardev stdio,id=char0,logfile=serial-1.log,signal=off,mux=on \
    -serial chardev:char0 \
	-monitor unix:$HOME/trampoline/qemu-sock,server,nowait \
    -monitor chardev:char0 2> qemu-stderr-1.log 

# restore the mapping
stty intr ^c
echo "VM stopped. Restored CTRL-C behaviour"

exit 0


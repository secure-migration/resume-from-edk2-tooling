#!/bin/bash

# map CTRL-C to CTRL ]
echo "Mapping CTRL-C to CTRL-]"
stty intr ^]

echo "Launching VM ..."

#sudo gdb -ex 'handle SIGUSR1 noprint' \
#	--args /home/dmurik/qemu/build/x86_64-softmmu/qemu-system-x86_64 \
sudo /home/dmurik/qemu/build/x86_64-softmmu/qemu-system-x86_64 \
	-name "vm-$USER" \
	-enable-kvm -cpu EPYC -machine q35 -m 1G \
	-drive if=pflash,format=raw,unit=0,file=$HOME/trampoline/edk2-internal/Build/AmdSev/DEBUG_GCC5/FV/AMDSEV.fd,readonly \
	-fw_cfg name=opt/ovmf/PcdSevIsMigrationHandler,string=1 \
	-fw_cfg name=opt/ovmf/PcdMigrationStateCR3,string=0x3a71c000 \
	-fw_cfg name=opt/ovmf/PcdMigrationStatePage,file=$HOME/trampoline-tooling/state-blob/cpu_state.bin \
	-drive file=$HOME/trampoline/ubuntu-mod.img,if=none,id=disk0,format=qcow2 \
	-device virtio-scsi-pci,id=scsi,disable-legacy=on,iommu_platform=true \
	-device scsi-hd,drive=disk0 \
    -nographic \
	-global isa-debugcon.iobase=0x402 -debugcon file:ovmf-2.log \
	-qmp unix:$HOME/trampoline/qmp-sock,server,nowait \
    -chardev stdio,id=char0,logfile=serial-2.log,signal=off,mux=on \
    -serial chardev:char0 \
	-monitor unix:$HOME/trampoline/qemu-sock,server,nowait \
    -monitor chardev:char0 \
	-loadvm save3 2> qemu-stderr-2.log

# restore the mapping
# stty intr ^c
# echo "VM stopped. Restored CTRL-C behaviour"

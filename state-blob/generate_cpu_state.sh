#!/bin/bash

set -e

EDK2_INCLUDE_DIR=$HOME/trampoline/edk2-internal/AmdSevPkg/SevLaunchSecret/MigrationHandler
CFLAGS="-I${EDK2_INCLUDE_DIR} -g"

MODIFY_STATE=""
if [ "$1" == "--escape-zzzloop" ] ; then
  MODIFY_STATE="--rip-offset 16"
fi

./process_qemu_info_registers.py $MODIFY_STATE < qemu.txt > generated-state.i
gcc $CFLAGS state-blob.c -o state-blob
./state-blob
# Now we should have cpu_state.bin

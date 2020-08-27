#!/bin/bash

QMPSOCK="$HOME/trampoline/qmp-sock"

clear
echo '+ Split window and run QEMU'
tmux split-window \; last-pane
NEW_PANE_ID=$(tmux list-panes -F '#{pane_id}' | tail -1)
echo '+ Attempting VM snapshot restore'
tmux send-keys -t $NEW_PANE_ID './restore_snapshot_plain_vm.sh' Enter
echo '+ Wait for OVMF Migration Handler'
sleep 5
echo '+ Reload memory into VM'
sudo ./reload-saved-memory-qmp.py $QMPSOCK
echo '+ Finished restore'
echo
echo
echo 'Run this to kill the VM:'
echo
echo "sudo ./send-qmp-cmd.py $QMPSOCK quit"
echo

#!/bin/bash

QMPSOCK="$HOME/trampoline/qmp-sock"

clear
echo '+ Split window and run QEMU'
tmux split-window \; last-pane
NEW_PANE_ID=$(tmux list-panes -F '#{pane_id}' | tail -1)
tmux send-keys -t $NEW_PANE_ID './run_plain_vm.sh -nonet' Enter
echo '+ Wait for the grub prompt'
sleep 8
echo '+ Send [Enter] to the VM'
tmux send-keys -t $NEW_PANE_ID Enter
echo '+ Wait for Linux to load'
sleep 12
echo '+ Login to linux'
tmux send-keys -t $NEW_PANE_ID 'root' Enter
sleep 0.5
tmux send-keys -t $NEW_PANE_ID 'gocubsgo' Enter
sleep 1
tmux send-keys -t $NEW_PANE_ID 'cat /proc/zzzloop' Enter
#tmux send-keys -t $NEW_PANE_ID 'cat /proc/uptime' Enter
sleep 0.5
echo '+ Pause the VM and record its memory and CPU state'
sudo ./snap-vm-qmp.py $QMPSOCK > $HOME/trampoline-tooling/state-blob/qemu.txt
make -C $HOME/trampoline-tooling/state-blob
sleep 1
echo '+ Attempting VM snapshot restore'
tmux send-keys -t $NEW_PANE_ID './restore_snapshot_plain_vm.sh' Enter
echo '+ Wait for OVMF Migration Handler'
sleep 5
echo '+ Reload memory into VM'
sudo ./reload-saved-memory-qmp.py $QMPSOCK
echo '+ Wait for final restore'
sleep 5
echo '+ Send quit command to the VM'
sudo ./send-qmp-cmd.py $QMPSOCK quit
sleep 1
echo '+ Close the other pane'
tmux send-keys -t $NEW_PANE_ID 'exit' Enter
echo
echo
echo 'Now you can run:'
echo
echo './read-ovmf-log.py | less -pMIGR.*calling.RestoreStep1'
echo
echo

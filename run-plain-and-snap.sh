#!/bin/bash

QMPSOCK="$HOME/trampoline/qmp-sock"
ENTER_ZZZLOOP=0
CPU_STATE_ARGS=""
if [ "$1" = "--zzzloop" ] ; then
  ENTER_ZZZLOOP=1
  CPU_STATE_ARGS="--escape-zzzloop"
fi

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
#echo '+ Start user-mode timer in source VM'
#tmux send-keys -t $NEW_PANE_ID '/root/usertimer.py' Enter
#sleep 0.5
#echo '+ Start kernel timers in source VM'
#tmux send-keys -t $NEW_PANE_ID 'cat /proc/zzzloop_apic_flag_on /proc/zzzloop_timer_start /proc/uptime' Enter
#sleep 2
if [ "$ENTER_ZZZLOOP" = "1" ] ; then
  echo '+ Enter zzzloop in source VM'
  tmux send-keys -t $NEW_PANE_ID 'cat /proc/zzzloop' Enter
  sleep 0.5
fi
echo '+ Pause the VM and record its memory and CPU state'
sudo rm -f devices-state.bin hpet-mem-dump.bin mem-dump-1.bin mem-dump-2.bin mem-dump.bin
sudo ./snap-vm-qmp.py $QMPSOCK > $HOME/trampoline-tooling/state-blob/qemu.txt
(cd state-blob && ./generate_cpu_state.sh $CPU_STATE_ARGS)

echo 'Now you can run:'
echo
echo './just-restore-from-snap.sh'
echo

# Remove this to perform automatic restore:
exit 0



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

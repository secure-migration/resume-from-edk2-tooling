EDK2_INCLUDE_DIR = $(HOME)/trampoline/edk2-internal/AmdSevPkg/ResumeFromEfi
CFLAGS += -I$(EDK2_INCLUDE_DIR) -g

cpu_state.bin: state-blob
	./state-blob

state-blob: state-blob.c generated-state.i
	gcc $(CFLAGS) state-blob.c -o state-blob

generated-state.i: qemu.txt ./process_qemu_info_registers.py
	./process_qemu_info_registers.py < $< > $@

.PHONY: clean
clean:
	rm -f generated-state.i state-blob cpu_state.bin

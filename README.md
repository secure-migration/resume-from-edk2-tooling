# Resume from EFI 

This is a demo showing how OVMF can load a VM snapshot. Normally the hypervisor would handle resuming a snapshot, but with SEV(-ES) the hypervisor is outside the trust domain of the guest so we resume from OVMF instead. This repository mainly contains tooling that you will need to run the demo. There is a separate EDK2 tree where Resume from EFI is implemented and a separate QEMu tree with a few temporary changes. 

## What does the demo do? 

1. Run a source VM 
2. Stop the source VM and take a snapshot
3. HV dumps memory to file
4. HV dumps device state to file
5. Use (modified) info registers
6. Start the CPU state from the source 
7. Stop source VM 
8. Start target VM from snapshot 
9. fw_cfg redirects target to Resume from EFI UEFI Application and passes in CPU state from the source 
10. Resume from EFI starts trampoline
11. HV loads memory from file (to reconcile source memory that was overwritten by OVMF)
12. HV loads device state from file (to reconcile devices that were reset by OVMF)
13. Trampoline restores registers and returns into Linux 

As discussed in the overview, a number of these steps violate the SEV trust model. In the overview we discuss how a snapshot can be transmitted while using SEV.

## How to run 

1. Checkout submodules
2. Build QEMU 
    1. `mkdir build`
    2. `cd build`
    3. `../configure --target-list=x86_64-softmmu`
3. Build OVMF
    1. Checkout submodules
    2. `make -C BaseTools`
    3. `source edksetup.sh`
    4. `build -t GCC5 -a X64 -p AmdSevPkg/AmdSevPkg.dsc`
4. Create OS image
    1. Disable KASLR
    2. Update `run_plain_vm.sh` with image path/name
    3. Update `run-pain-vm-and-snap.sh` with image credentials (see comment in script)
5. Run `run-plain-vm-and-snap.sh`
6. Run `just-restore-from-snap.sh`

## What changes did we make? 

Our changes are spread out over many commits in three different repositories. Here is a summary of what we did.

Most of our effort went into EDK2. Specifically we created a new package based on OVMF called AmdSevPkg. We created this package for convenience. Resume from EFI can be integrated into OVMF. The majority of code for Resume from EFI lives in `edk2/AmdSevPkg/ResumeFromEfi`. Some of the code inside this directory refers to a MigrationHandler. This is a placeholder for some of the functionality that would be incorporated into a future implementation.

We also provide a modified version of QEMU. These modifications are not intended for upstream, rather they make some adjustments that the tools in this repo depend on. For example, we modify loadvm so that it doesn't restore the CPU state. We also modify info registers to show more CPU state than it normally does. 

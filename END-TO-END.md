The following is a rough outline of how live migration for SEV-ES would work. 

1. Startup
    1. Start the source VM
        1. Attest the source memory and CPU state (for SEV-ES)
        2. Map VMSA
            1. Hypervisor adds entry to NPT that maps the HPA of the VMSA (found in the VMCB) to some known GPA. 
            2. OVMF reserves the GPA of the VMSA so that the OS can't overwrite it.
2. Init migration
    1. Start the target
        1. Target does SEV attested startup
        2. provide fw_config to target to divert to migration handler. 
            1. Migration Handler for memory must live within runtime memory so that it will not overwrite itself in step 3
        3. Start APs and put them into some kind of halt (waiting for a VMRun, perhaps)
            1. we haven't quite decided the best way to handle the APs, but it is probably best to start them with OVMF's multiprocessing service before we start receiving pages
    2. Prepare source for migration. One vCPU will need to be set aside for the migration handler. You can set this up either in step 1 or here.
3. Memory transfer
    1. the source hypervisor asks the migration handler to encrypt pages with the transport key. communication is done via a mailbox (see AMD patch for this). migration handler will probably need to map all of memory. the hypervisor will send the encrypted pages over the network to the destination.
    2. the target hypervisor will receive encrypted pages and ask the migration handler to decrypt them 
    3. this process will continue until we meet the threshold of convergence. at some point(s) the VMSAs for each vCPU will be copied to the destination (although these might not have the final cpustate
4. Convergence
    1. Source HV sends device state to destination
    2. Source VMExits to update VMSA 
    3. HV resends pages with VMSA to destination
    4. destination loads device state and begins trampoline
5. Trampoline
    1. Each vCPU/AP locates its VMSA and trampolines into Linux

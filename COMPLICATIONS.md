The following are issues that we have taken a shortcut around in this demo (besides transmission of the snapshot, which is discussed in the overview). 

## Multiprocessing
The demo migrates VMs with one vCPU. To handle VMs with more vCPUs we will need to run the trampoline on each vCPU. The state of each vCPU will be stored in a separate VMSA. We will probably use OVMF's multiprocessing service, which allows us to run code on each AP in a relatively rich environment. This service is only available at boot time, however, so once we start copying pages, it might get overwritten. We will probably need to start the APs before copying memory and then have them halt until we are ready.

## Memory coordination
As alluded to above, we have to be careful when it comes to the memory footprint of our migrations tools in OVMF.  Anything on the target that lives outside the footprint of OVMF on the source will likely be overwritten when we transfer memory. OVMF on the source will have already called ExitBootServices by the time we migrate, meaning that boot memory is not protected. Everything we want to use on the target needs to be reserved as runtime code/data. Currently our resume from EFI demo is setup as a UEFI Application, which doesn't meet these requirements. Memeory coordination should not cause any unsurmountable issues, but we will need to be deliberate about how we layout our system.u

## Intermediate Mappings (KASLR)
We turn KASLR off for the demo because it makes it more difficult to calculate the source virtual addresses of the pages for stage 2 and 3 of the trampoline. We need to know the VA of these pages in order to generate intermediate mappings.

## Source Mappings
The trampoline page that switches to the source CR3 and any pages that run after it, need to be mapped in the page table tree that the source CR3 points to. Currently we don't coordinate with Linux to explicitly map these pages. Instead, we just mark them as boot code (so they will be available to the kernel) and hope the kernel doesn't overwrite them. We sandwich the pages with runtime code (not accessible to the kernel), which might offer some protection. For a real version, we need the kernel to be aware of these pages and set them aside so they can't be overwritten.

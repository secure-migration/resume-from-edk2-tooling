#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// basic helpers 
// from include/uapi/linux/const.h
#define _AT(T,X)    ((T)(X))
#define __AC(X,Y)   (X##Y)
#define _AC(X,Y)    __AC(X,Y)

#define CR3_ADDR_MASK 0x7FFFFFFFFFFFF000ull
#define cr3_to_pgt_pa(cr3) ((unsigned long)(cr3) & CR3_ADDR_MASK)

#define PAGE_OFFSET 0xffff888000000000ull
#define KERNEL_OFFSET 0x1000000

// we made this macro. use at your own risk. 
#define K_OFFSET(address) (PAGE_OFFSET + KERNEL_OFFSET + address)

#define __pa(x)     __phys_addr((unsigned long)(x))
#define __va(x)         ((void *)((unsigned long)(x)+PAGE_OFFSET))

typedef unsigned long UINT64;
typedef unsigned long phys_addr_t;

// basic types for pages
typedef unsigned long   pteval_t;

typedef unsigned long   pmdval_t;
typedef unsigned long   pudval_t;
typedef unsigned long   pteval_t;
typedef unsigned long   pgdval_t;

#define pgd_val(x)  ((x).pgd)
#define pud_val(x)  ((x).pud)
#define pmd_val(x)  ((x).pmd)
#define pte_val(x)  ((x).pte)

typedef struct { pgdval_t pgd; } pgd_t;
typedef struct { pudval_t pud; } pud_t;
typedef struct { pmdval_t pmd; } pmd_t;
typedef struct { pmdval_t pte; } pte_t;

#define __pgd(x)    ((pgd_t) { (x) } )
#define __pmd(x)    ((pmd_t) { (x) } )
#define __pud(x)    ((pud_t) { (x) } )
#define __pte(x)    ((pte_t) { (x) } )

#define pgd_none(pgd)       (!pgd_val(pgd))
#define pud_none(pud)       (!pud_val(pud))
#define pmd_none(pmd)       (!pmd_val(pmd))



// arch/x86/include/asm/pgtable_types.h

// A bunch of page types from arch/x86/incude/asm/pgtable_types.h
// We probably don't need every single one.
#define _PAGE_BIT_PRESENT   0   /* is present */
#define _PAGE_BIT_RW        1   /* writeable */
#define _PAGE_BIT_USER      2   /* userspace addressable */
#define _PAGE_BIT_PWT       3   /* page write through */
#define _PAGE_BIT_PCD       4   /* page cache disabled */
#define _PAGE_BIT_ACCESSED  5   /* was accessed (raised by CPU) */
#define _PAGE_BIT_DIRTY     6   /* was written to (raised by CPU) */
#define _PAGE_BIT_PSE       7   /* 4 MB (or 2MB) page */
#define _PAGE_BIT_PAT       7   /* on 4KB pages */
#define _PAGE_BIT_GLOBAL    8   /* Global TLB entry PPro+ */
#define _PAGE_BIT_SOFTW1    9   /* available for programmer */
#define _PAGE_BIT_SOFTW2    10  /* " */
#define _PAGE_BIT_SOFTW3    11  /* " */
#define _PAGE_BIT_PAT_LARGE 12  /* On 2MB or 1GB pages */
#define _PAGE_BIT_SOFTW4    58  /* available for programmer */
#define _PAGE_BIT_PKEY_BIT0 59  /* Protection Keys, bit 1/4 */
#define _PAGE_BIT_PKEY_BIT1 60  /* Protection Keys, bit 2/4 */
#define _PAGE_BIT_PKEY_BIT2 61  /* Protection Keys, bit 3/4 */
#define _PAGE_BIT_PKEY_BIT3 62  /* Protection Keys, bit 4/4 */
#define _PAGE_BIT_NX        63  /* No execute: only valid after cpuid check */

// shift over the above 
#define _PAGE_PRESENT   (_AT(pteval_t, 1) << _PAGE_BIT_PRESENT)
#define _PAGE_RW    (_AT(pteval_t, 1) << _PAGE_BIT_RW)
#define _PAGE_USER  (_AT(pteval_t, 1) << _PAGE_BIT_USER)
#define _PAGE_PWT   (_AT(pteval_t, 1) << _PAGE_BIT_PWT)
#define _PAGE_PCD   (_AT(pteval_t, 1) << _PAGE_BIT_PCD)
#define _PAGE_ACCESSED  (_AT(pteval_t, 1) << _PAGE_BIT_ACCESSED)
#define _PAGE_DIRTY (_AT(pteval_t, 1) << _PAGE_BIT_DIRTY)
#define _PAGE_PSE   (_AT(pteval_t, 1) << _PAGE_BIT_PSE)
#define _PAGE_GLOBAL    (_AT(pteval_t, 1) << _PAGE_BIT_GLOBAL)
#define _PAGE_SOFTW1    (_AT(pteval_t, 1) << _PAGE_BIT_SOFTW1)
#define _PAGE_SOFTW2    (_AT(pteval_t, 1) << _PAGE_BIT_SOFTW2)
#define _PAGE_SOFTW3    (_AT(pteval_t, 1) << _PAGE_BIT_SOFTW3)
#define _PAGE_PAT   (_AT(pteval_t, 1) << _PAGE_BIT_PAT)
#define _PAGE_PAT_LARGE (_AT(pteval_t, 1) << _PAGE_BIT_PAT_LARGE)
#define _PAGE_SPECIAL   (_AT(pteval_t, 1) << _PAGE_BIT_SPECIAL)
#define _PAGE_CPA_TEST  (_AT(pteval_t, 1) << _PAGE_BIT_CPA_TEST)
#define _PAGE_NX        (_AT(pteval_t, 1) << _PAGE_BIT_NX)

// this is slightly odd
#define sme_me_mask  0ULL
#define _PAGE_ENC       (_AT(pteval_t, sme_me_mask))

// rename
#define __PP _PAGE_PRESENT
#define __RW _PAGE_RW
#define _USR _PAGE_USER
#define ___A _PAGE_ACCESSED
#define ___D _PAGE_DIRTY
#define ___G _PAGE_GLOBAL
#define __NX _PAGE_NX

#define _ENC _PAGE_ENC
#define __WP _PAGE_CACHE_WP
#define __NC _PAGE_NOCACHE
#define _PSE _PAGE_PSE

// more complex types
#define PAGE_NONE        __pg(   0|   0|   0|___A|   0|   0|   0|___G)
#define PAGE_SHARED      __pg(__PP|__RW|_USR|___A|__NX|   0|   0|   0)
#define PAGE_SHARED_EXEC     __pg(__PP|__RW|_USR|___A|   0|   0|   0|   0)
#define PAGE_COPY_NOEXEC     __pg(__PP|   0|_USR|___A|__NX|   0|   0|   0)
#define PAGE_COPY_EXEC       __pg(__PP|   0|_USR|___A|   0|   0|   0|   0)
#define PAGE_COPY        __pg(__PP|   0|_USR|___A|__NX|   0|   0|   0)
#define PAGE_READONLY        __pg(__PP|   0|_USR|___A|__NX|   0|   0|   0)
#define PAGE_READONLY_EXEC   __pg(__PP|   0|_USR|___A|   0|   0|   0|   0)

#define __PAGE_KERNEL        (__PP|__RW|   0|___A|__NX|___D|   0|___G)
#define __PAGE_KERNEL_EXEC   (__PP|__RW|   0|___A|   0|___D|   0|___G)
#define _KERNPG_TABLE_NOENC  (__PP|__RW|   0|___A|   0|___D|   0|   0)
#define _KERNPG_TABLE        (__PP|__RW|   0|___A|   0|___D|   0|   0| _ENC)
#define _PAGE_TABLE_NOENC    (__PP|__RW|_USR|___A|   0|___D|   0|   0)
#define _PAGE_TABLE      (__PP|__RW|_USR|___A|   0|___D|   0|   0| _ENC)
#define __PAGE_KERNEL_RO     (__PP|   0|   0|___A|__NX|___D|   0|___G)
#define __PAGE_KERNEL_RX     (__PP|   0|   0|___A|   0|___D|   0|___G)
#define __PAGE_KERNEL_NOCACHE    (__PP|__RW|   0|___A|__NX|___D|   0|___G| __NC)
#define __PAGE_KERNEL_VVAR   (__PP|   0|_USR|___A|__NX|___D|   0|___G)
#define __PAGE_KERNEL_LARGE  (__PP|__RW|   0|___A|__NX|___D|_PSE|___G)
#define __PAGE_KERNEL_LARGE_EXEC (__PP|__RW|   0|___A|   0|___D|_PSE|___G)
#define __PAGE_KERNEL_WP     (__PP|__RW|   0|___A|__NX|___D|   0|___G| __WP)

// The kernel does this weird thing for type checking 
// I don't entirely understand why or if we need both.
typedef struct { unsigned long pgprot; } pgprot_t;
//typedef unsigned long pgprot_t;
#define __pgprot(x)     ((pgprot_t) { (x) } )

#define pgprot_val(x)   ((x).pgprot)

// Masking and shifting

// hopefully 
#define PTE_ORDER 0
#define PGD_ORDER 0

#define PAGE_SHIFT 12 // 2^12 = 4096, I think this is what we want...
#define __PHYSICAL_MASK_SHIFT   52

#define PMD_SHIFT   (PAGE_SHIFT + (PAGE_SHIFT + PTE_ORDER - 3))
#define PMD_SIZE    (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK    (~(PMD_SIZE - 1))
#define PMD_BITS    (PAGE_SHIFT - 3)

#define PUD_SHIFT   (PMD_SHIFT + PMD_BITS)
#define PUD_SIZE    (_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK    (~(PUD_SIZE - 1))
#define PUD_BITS    (PAGE_SHIFT - 3)

#define PGDIR_SHIFT (PUD_SHIFT + PUD_BITS)
#define PGDIR_SIZE  (_AC(1,UL) << PGDIR_SHIFT)
#define PGDIR_MASK  (~(PGDIR_SIZE-1))
#define PGDIR_BITS  (PAGE_SHIFT - 3)

#define ENTRIES 512

#define PTRS_PER_PMD ENTRIES
#define PTRS_PER_PUD ENTRIES
#define PTRS_PER_PGD ENTRIES
#define PTRS_PER_PTE ENTRIES

#define pgd_index(address)  (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define pud_index(address)  (((address) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define pmd_index(address)  (((address) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define pte_index(address)  ((address >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

#define PAGE_SIZE 4096
#define FIXMAP_PMD_NUM 2 // not totally sure about this

// not really sure about this
#define __default_kernel_pte_mask ~0

#define pgd_offset_pgd(pgd, address) (pgd + pgd_index((address)))

#define __PHYSICAL_MASK     (phys_addr_t)((1ULL << __PHYSICAL_MASK_SHIFT) - 1)
#define PAGE_MASK       (~(PAGE_SIZE-1))
#define PHYSICAL_PAGE_MASK  (((signed long)PAGE_MASK) & __PHYSICAL_MASK)
#define PTE_PFN_MASK        ((pteval_t)PHYSICAL_PAGE_MASK)

#define PUD_PAGE_SIZE       (_AC(1, UL) << PUD_SHIFT)
#define PUD_PAGE_MASK       (~(PUD_PAGE_SIZE-1))

#define PMD_PAGE_SIZE       (_AC(1, UL) << PMD_SHIFT)
#define PMD_PAGE_MASK       (~(PMD_PAGE_SIZE-1))

#define PHYSICAL_PMD_PAGE_MASK  (((signed long)PMD_PAGE_MASK) & __PHYSICAL_MASK)
#define PHYSICAL_PUD_PAGE_MASK  (((signed long)PUD_PAGE_MASK) & __PHYSICAL_MASK)

// helpers for pagetable walk

static inline pudval_t native_pud_val(pud_t pud)
{
	return pud.pud;
}

static inline pmdval_t native_pmd_val(pmd_t pmd)
{
	return pmd.pmd;
}

static inline pudval_t pud_pfn_mask(pud_t pud)
{
	if (native_pud_val(pud) & _PAGE_PSE)
		return PHYSICAL_PUD_PAGE_MASK;
	else
		return PTE_PFN_MASK;
}

static inline pudval_t pud_flags_mask(pud_t pud)
{
        return ~pud_pfn_mask(pud);
}

static inline pudval_t pud_flags(pud_t pud)
{
        return native_pud_val(pud) & pud_flags_mask(pud);
}

#define mk32 (((UINT64)1 << 32) - 1)
static inline pmdval_t pmd_pfn_mask(pmd_t pmd)
{
	if (native_pmd_val(pmd) & _PAGE_PSE)
		return PHYSICAL_PMD_PAGE_MASK;
	else
		return PTE_PFN_MASK;
}

static inline pmdval_t pmd_flags_mask(pmd_t pmd)
{
        return ~pmd_pfn_mask(pmd);
}

static inline pmdval_t pmd_flags(pmd_t pmd)
{
        return native_pmd_val(pmd) & pmd_flags_mask(pmd);
}

static inline unsigned long pgd_page_vaddr(pgd_t pgd)
{
	//return (unsigned long)__va((unsigned long)pgd_val(pgd) & PTE_PFN_MASK & mk32);
    return pgd_val(pgd) & PTE_PFN_MASK;
}

static inline unsigned long pud_page_vaddr(pud_t pud)
{
	return pud_val(pud) & pud_pfn_mask(pud);
}

static inline unsigned long pmd_page_vaddr(pmd_t pmd)
{
	return pmd_val(pmd) & pmd_pfn_mask(pmd);
}

/* Find an entry in the third-level page table.. */
static inline pud_t *pud_offset(void *physmem, pgd_t *pgd, unsigned long address)
{
	return (pud_t *)(physmem + pgd_page_vaddr(*pgd)) + pud_index(address);
}

/* Find an entry in the second-level page table.. */
static inline pmd_t *pmd_offset(void *physmem, pud_t *pud, unsigned long address)
{
	return (pmd_t *)(physmem + pud_page_vaddr(*pud)) + pmd_index(address);
}

static inline pte_t *pte_offset_kernel(void *physmem, pmd_t *pmd, unsigned long address)
{
	return (pte_t *)(physmem + pmd_page_vaddr(*pmd)) + pte_index(address);
}

#define PHYS_PGD(x) ((pgd_t *)(physmem + (unsigned long)(x)))
#define PHYS_PUD(x) ((pud_t *)(physmem + (unsigned long)(x)))
#define PHYS_PMD(x) ((pmd_t *)(physmem + (unsigned long)(x)))
#define PHYS_PTE(x) ((pte_t *)(physmem + (unsigned long)(x)))

int ClearPageNXFlag(void* physmem, UINT64 pgd_base, unsigned long va) {
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep;
    printf("MH: ClearPageNXFlag: VA 0x%lx in PGT at 0x%lx\n", va, pgd_base);

    pgd = pgd_offset_pgd((pgd_t*)(physmem + pgd_base), va);
    if (pgd_none(*pgd)) {
        printf("ClearPageNXFlag quitting > pgd value: %lx\n", pgd->pgd);
        return -1;
    }

    pud = pud_offset(physmem, pgd, va);
    if (pud_none(*pud)) {
        printf("ClearPageNXFlag quitting > pud value: %lx\n", pud_val(*pud));
        return -2;
    }

    pmd = pmd_offset(physmem, pud, va);
    if (pmd_none(*pmd)) {
        printf("ClearPageNXFlag quitting > pmd value: %lx\n", pmd_val(*pmd));
        return -3;
    }

    ptep = pte_offset_kernel(physmem, pmd, va);
    if (!ptep) {
        //printf("ClearPageNXFlag quitting > pte value: %lx\n", pte_val(*ptep));
        return -4;
    }
    printf("ClearPageNXFlag: pte entry address is: %p\n", (void *)ptep);
    printf("ClearPageNXFlag: pte value before: %lx\n", pte_val(*ptep));
    ptep->pte &= ~_PAGE_NX;
    printf("ClearPageNXFlag: pte value after: %lx\n", pte_val(*ptep));
    printf("msync\n");
    msync(ptep, sizeof(*ptep), MS_SYNC);

    return 0;
}

void modify_mem_dump(const char* filename, unsigned long cr3, unsigned long va) {
    printf("Opening %s\n", filename);
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    struct stat st;
    fstat(fd, &st);
    printf("File size: %ld bytes\n", st.st_size);

    printf("mmap\n");
    void* physmem = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int rc = ClearPageNXFlag(physmem, cr3, va);
    printf("ClearPageNXFlag returned %d\n", rc);

    printf("modify TSS descriptor in GDT\n");
    unsigned long gdt_phys_addr = 0x3f80b000;
    unsigned long tss_offset = 0x40; // 8th entry in GDT
    unsigned long *tss_desc = (unsigned long *)(physmem + gdt_phys_addr + tss_offset);
    printf("before: tss_desc = %016lx\n", *tss_desc);
    *tss_desc &= ~(1UL << 41); // Clear 41-h bit
    printf("after : tss_desc = %016lx\n", *tss_desc);
    printf("msync\n");
    msync(tss_desc, sizeof(*tss_desc), MS_SYNC);

    printf("munmap\n");
    munmap(physmem, st.st_size);
    printf("close\n");
    close(fd);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <mem_dump_filename> <cr3> <va>\n", argv[0]);
        return 1;
    }
    unsigned long cr3 = strtoul(argv[2], NULL, 0);
    unsigned long va = strtoul(argv[3], NULL, 0);
    modify_mem_dump(argv[1], cr3, va);
    return 0;
}

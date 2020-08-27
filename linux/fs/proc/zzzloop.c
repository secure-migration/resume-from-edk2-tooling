// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/time_namespace.h>
#include <linux/kernel_stat.h>

#define NOP_SLIDE \
        __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop"); \
        __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop"); \
        __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop"); \
        __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop")

// Send "LNX1\r\n" to the debug port 0x402
#define DBG_OUT_LNX1 \
	__asm__ __volatile__ (      \
                "mov $0x402, %dx\n" \
                "mov $0x4c, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x4e, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x58, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x31, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x0d, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x0a, %al\n"  \
                "out %al, (%dx)\n"  \
                )

// Send "LNX2\r\n" to the debug port 0x402
#define DBG_OUT_LNX2 \
	__asm__ __volatile__ (      \
                "mov $0x402, %dx\n" \
                "mov $0x4c, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x4e, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x58, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x32, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x0d, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x0a, %al\n"  \
                "out %al, (%dx)\n"  \
                )

// Send "LNX3\r\n" to the debug port 0x402
#define DBG_OUT_LNX3 \
	__asm__ __volatile__ (      \
                "mov $0x402, %dx\n" \
                "mov $0x4c, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x4e, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x58, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x33, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x0d, %al\n"  \
                "out %al, (%dx)\n"  \
                "mov $0x0a, %al\n"  \
                "out %al, (%dx)\n"  \
                )

int zzzloop_debugprint(void)
{
        __asm__ __volatile__ (
                "mov $0x402, %dx\n"
                "mov $0x5a, %al\n"
                "out %al, (%dx)\n"
                "mov $0x59, %al\n"
                "out %al, (%dx)\n"
                "mov $0x58, %al\n"
                "out %al, (%dx)\n"
                );
        return 0;
}
EXPORT_SYMBOL(zzzloop_debugprint);

int zzzloop_main(void)
{
        volatile int run1 = 1;

	printk(KERN_ERR "zzzloop: Entered zzzloop_main 100\n");
	printk(KERN_ERR "zzzloop: Starting endless loop 200\n");
        while (run1) {
                __asm__ __volatile__ ("pause");
        }
        NOP_SLIDE;

        __asm__ __volatile__ ("zzzloop_continue:\n");
        DBG_OUT_LNX1;
	printk(KERN_ERR "zzzloop: After zzzloop_continue 300\n");
        DBG_OUT_LNX2;
        return 0;
}
EXPORT_SYMBOL(zzzloop_main);

int zzzloop2_main(void)
{
        volatile int run1 = 1;
        volatile int run2 = 1;

        printk(KERN_ERR "zzzloop2: Entered zzzloop_main 100\n");
        printk(KERN_ERR "zzzloop2: Starting endless loop 200\n");
        while (run1) {
                __asm__ __volatile__ ("pause");
        }
        NOP_SLIDE;

        __asm__ __volatile__ ("zzzloop2_continue:\n");

        DBG_OUT_LNX1;
        printk(KERN_ERR "zzzloop2: After zzzloop2_continue 300\n");
        DBG_OUT_LNX2;

        printk(KERN_ERR "zzzloop2: Starting second endless loop 400\n");
        while (run2) {
                __asm__ __volatile__ ("pause");
        }
        NOP_SLIDE;

        __asm__ __volatile__ ("zzzloop2_continue_2:\n");
        DBG_OUT_LNX3;
        printk(KERN_ERR "zzzloop2: After zzzloop2_continue_2\n");
        return 0;
}
EXPORT_SYMBOL(zzzloop2_main);

static int zzzloop_proc_show(struct seq_file *m, void *v)
{
	int i = 0;

	seq_printf(m, "Test zzzloop i=%d\n", i);
        zzzloop_main();
	return 0;
}

static int zzzloop2_proc_show(struct seq_file *m, void *v)
{
	int i = 0;

	seq_printf(m, "Test zzzloop2 i=%d\n", i);
        zzzloop2_main();
	return 0;
}

static int __init proc_zzzloop_init(void)
{
	proc_create_single("zzzloop", 0, NULL, zzzloop_proc_show);
	proc_create_single("zzzloop2", 0, NULL, zzzloop2_proc_show);
	return 0;
}
fs_initcall(proc_zzzloop_init);

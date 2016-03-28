/*
 * Copyright (c) 2016 Evgeny Yulyugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/msr.h>

#include "smigen.h"
#include "smigen-ioctl.h"

MODULE_LICENSE("GPL v3");
MODULE_AUTHOR("Evgeny Yulyugin <yulyugin@gmail.com>");
MODULE_DESCRIPTION("smigen");

static int
smigen_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int
smigen_release(struct inode *inode, struct file *file)
{
    return 0;
}

static int
smigen_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
             unsigned long param)
{
    switch(cmd) {
    case SMIGEN_STOP:
        smigen_printk("SMIGEN_STOP: not yet implemented\n");
        break;

    case SMIGEN_START:
        smigen_printk("SMIGEN_START: %d\n", param);\
        smigen_trigger_smi();
        break;

    default:
        smigen_printk("Unrecognised ioctl request %#llx\n", cmd);
        return -EINVAL;
    }
    return 0;
}

static const struct file_operations smigen_fops = {
    .owner = THIS_MODULE,
    .open = smigen_open,
    .release = smigen_release,
    .ioctl = smigen_ioctl,
};

static struct miscdevice smigen_dev = {
    MISC_DYNAMIC_MINOR,
    "smigen",
    &smigen_fops,
};

static int __init
smigen_init(void)
{
    int ret = misc_register(&smigen_dev);
    if (ret)
        smigen_printk("Unable to register smigen device.\n");
    return ret;
}

static void __exit
smigen_exit(void)
{
    misc_deregister(&smigen_dev);
}

module_init(smigen_init);
module_exit(smigen_exit);

int
smigen_printk(const char *fmt, ...)
{
    int ret;
    va_list va;
    va_start(va, fmt);
    ret = vprintk(fmt, va);
    va_end(va);
    return ret;
}

int
smigen_safe_rdmsr(unsigned msr, uint64 *val)
{
    return rdmsrl_safe(msr, val);
}

void
smigen_port_out(unsigned port, uint32 data)
{
    __asm__ __volatile__(
        "outl %0, %w1"
        :: "a"(data), "Nd"(port):
        );
}


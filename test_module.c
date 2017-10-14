#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>

#define QUESIZE 100

/*data structure declearation for this module*/
struct io_desc {
	struct timespec time;
	sector_t sector_num;
};

/*global variables*/
struct io_desc cir_que[QUESIZE];
int que_count = 0;
struct proc_dir_entry *my_proc_dir;
struct proc_dir_entry *my_proc_file;
struct file_operations myproc_fops = { .owner = THIS_MODULE };

static int __init init_my_module(void) {
	my_proc_dir = proc_mkdir("oslab_dir", NULL);
	my_proc_file = proc_create("oslab_file", 0600, my_proc_dir, &myproc_fops);
	

	printk(KERN_ALERT "module starts.\n");
	return 0;
}

static void __exit exit_my_module(void) {
	printk(KERN_ALERT "module terminates\n");
	return;
}

module_init(init_my_module);
module_exit(exit_my_module);

MODULE_AUTHOR("SUWAN KIM");
MODULE_DESCRIPTION("FIRST MADE KERNEL MODULE");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");

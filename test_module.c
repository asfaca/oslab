#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/time.h>

#define QUESIZE 100

/*declearation for this module*/
struct myio_desc {
	struct timespec time;
	sector_t sector_num;
};

struct myio_cir_que {
	struct myio_desc que[QUESIZE];
	int que_count;
	int fir_index;
	int curr_index;
}

static int my_open(struct inode *inode, struct file *file) {
	printk(KERN_ALERT, "my open function\n");
	return 0;
}

static int my_write(struct file *file, const char __user *user_buffer, 
		    size_t count, loff_t *ppos) {
	printk(KERN_ALERT, "my write function\n");
	return 0;
}


/*global variables*/
struct myio_cir_que myioque; 
EXPORT_SYMBOL(myioque);
struct proc_dir_entry *my_proc_dir;
struct proc_dir_entry *my_proc_file;
struct file_operations myproc_fops = { .owner = THIS_MODULE,
				       .open = my_open, 
				       .write = my_write,
};
struct timespec my_bio_time;
EXPORT_SYMBOL(my_bio_time);

static int __init init_my_module(void) {
	/*init circular queue*/
	myioque.que_count = 0;
	myioque.fir_index = 0;
	mtioque.curr_index = 0;
	
	/*create proc*/
	my_proc_dir = proc_mkdir("oslab_dir", NULL);
	my_proc_file = proc_create("oslab_file", 0600, my_proc_dir, &myproc_fops);
	
	printk(KERN_ALERT "module starts.\n");
	return 0;
}

static void __exit exit_my_module(void) {
	printk(KERN_ALERT "module terminates\n");
	return;
}

int add_myioque(struct myio_cir_que *que, struct timespec *time, ) {
	if (que->que_count == QUESIZE)
		print_que_to_proc(que); /*at here, initatilize que member like count, index ...*/

	/*store data*/
}


module_init(init_my_module);
module_exit(exit_my_module);

MODULE_AUTHOR("SUWAN KIM");
MODULE_DESCRIPTION("FIRST MADE KERNEL MODULE");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");

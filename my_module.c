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
};

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
	remove_proc_entry("oslab_file", my_proc_dir);
	remove_proc_entry("oslab_dir", NULL);
	printk(KERN_ALERT "module terminates\n");
	return;
}

int print_que_to_proc(struct myio_cir_que *que, struct proc_dir_entry *file) {
	/*write queue data to proc file system
	  if writing fail, return error code -1*/

	/*init queue*/
	que->que_count = 0;
	que->fir_index = 0;
	que->curr_index = 0;
	
	return 0;
}

int add_myioque(struct myio_cir_que *que, struct bio *bio, struct proc_dir_enrty *file) {
	/*need routine for exception of print_que_to_proc failure*/
	if (que->que_count == QUESIZE) {
		if (print_que_to_proc(que, file) < 0)
			return -1;
	}
	/*store data*/
	if (++que->curr_index == QUESIZE)
		que->curr_index = 0;
	/*get current time and store data*/
	getnssectimeofday(&my_bio_time);
	que->que[que->curr_index].time.tv_sec = my_bio_time.tv_sec;
	que->que[que->curr_index].time.tv_nsec = my_bio_time.tv_nsec;
	/*store sector address*/
	que->que[que->curr_index].sector_num = bio->bi_iter.bi_sector;
	que->que_count++;	
	
	return 0;
}
EXPORT_SYMBOL(add_myioque);

module_init(init_my_module);
module_exit(exit_my_module);

MODULE_AUTHOR("SUWAN KIM");
MODULE_DESCRIPTION("FIRST MADE KERNEL MODULE");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");

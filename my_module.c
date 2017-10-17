#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <asm/uaccess.h>

#define QUESIZE 100 
#define PROCSIZE 100000

/*declearation for this module*/
struct myio_desc {
	char *name;
	struct timespec time;
	sector_t sector_num;
};

struct myio_cir_que {
	struct myio_desc que[QUESIZE];
	int que_count;
	int fir_index;
	int curr_index;
};

struct myio_cir_que myioque; 

static char my_proc_buf[PROCSIZE];
int my_curr_fp = 0;

static int my_open(struct inode *inode, struct file *file) {
	my_curr_fp = 0;
	printk("my open function\n");
	return 0;
}

static ssize_t my_write(struct file *file, const char __user *user_buffer, 
		   size_t count, loff_t *ppos) {
	int i;
	int namesize = sizeof(myioque->que[0].name);
	int timesize = sizeof(myioque->que[0].time.tv_sec);
	int blknumsize = sizeof(myioque->que[0].sector_num);

	/*check proc buffer size*/
	if (my_curr_fp + namesize + timesize + blknumsize >= PROCSIZE)
		return 0;

	for (i = 0; i < QUESIZE; i++) {
		if (copy_from_user(&my_proc_buf[my_curr_fp], myioque->que[i].name, namesize)) {
			return -EFAULT;
		}
		my_curr_fp += namesize;
		if (copy_from_user(&my_proc_buf[my_curr_fp], myioque->que[i].time.tv_sec, timesize)) {
			return -EFAULT;
		}
		my_curr_fp += timesize;
		if (copy_from_user(&my_proc_buf[my_curr_fp], myioque->que[i].sector_num, blknumsize)) {
			return -EFAULT;
		}
		my_curr_fp += blknumsize;
	}

	/*init queue*/	
	que->que_count = 0;
	que->fir_index = 0;
	que->curr_index = 0;

	printk("my write function\n");
	return 0;
}

static ssize_t my_read(struct file * f, char __user * userArray, size_t s, loff_t * l){

	if(s >= sizeof(my_proc_buf)){
        memcpy(userArray, my_proc_buf, sizeof(my_proc_buf)); 	
       }

	else {
	printk("error : userArray size is smaller than my_proc_buf's size\n");
	}
}


/*global variables*/
struct proc_dir_entry *my_proc_dir;
struct proc_dir_entry *my_proc_file;
struct file_operations myproc_fops = { .owner = THIS_MODULE,
				       .open = my_open, 
				       .write = my_write,
				       .read = my_read,
};
struct timespec my_bio_time;

static int __init init_my_module(void) {
	/*init circular queue*/
	myioque.que_count = 0;
	myioque.fir_index = 0;
	myioque.curr_index = 0;
	
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


int add_myioque(struct bio *bio) {
	struct myio_cir_que *que = myioque;
	struct proc_dir_entry *file = my_proc_file;
	/*need routine for exception of print_que_to_proc failure*/
	if (que->que_count == QUESIZE) {
		if (file->proc_fops->write(NULL,NULL,0,NULL)) < 0)
			return -1;
	}
	/*store data*/
	if (++que->curr_index == QUESIZE)
		que->curr_index = 0;
	/*store file system name*/
	que->que[que->curr_index].name = bio->bi_bdev->bd_super->s_type->name;
	/*get current time and store data*/
	getnstimeofday(&my_bio_time);
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

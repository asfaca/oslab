#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#define QUESIZE 100 
#define PROCSIZE 100000
#define ASCIENTER 0x0A
#define ASCISPACE 0x20

/*declearation for this module*/
struct myio_desc {
	const char *name;
	struct timespec time;
	sector_t sector_num;
};

struct myio_cir_que {
	struct myio_desc que[QUESIZE];
	int que_count;
	int fir_index;
	int curr_index;
};

static struct myio_cir_que myioque; 
//EXPORT_SYMBOL(myioque);

static char my_proc_buf[PROCSIZE] = "";
int my_proc_fp = 0;

int my_open(struct inode *inode, struct file *file) {
	my_proc_fp = 0;
	printk("my open function\n");
	return 0;
}

ssize_t my_write(struct file *file, const char __user *user_buffer, 
		   size_t count, loff_t *ppos) {
	int i = myioque.curr_index;
	//test
	if (myioque.que_count == 0) {
		printk("error : que is empty. Nothing to write.\n");
		return -1;
	}

	/*check proc buffer size*/
	if (my_proc_fp > PROCSIZE-100) 
		return count;
	
	/*print data to proc*/
	while(1) {
		if (i == QUESIZE)
			i = 0;

		my_proc_fp += sprintf(&my_proc_buf[my_proc_fp], "%s", myioque.que[i].name);
		my_proc_buf[my_proc_fp++] = ASCISPACE;
		my_proc_fp += sprintf(&my_proc_buf[my_proc_fp], "%ld", myioque.que[i].sector_num);
		my_proc_buf[my_proc_fp++] = ASCISPACE;
		my_proc_fp += sprintf(&my_proc_buf[my_proc_fp], "%ld", myioque.que[i].time.tv_sec);
		my_proc_buf[my_proc_fp++] = ASCIENTER;

		if (--myioque.que_count == 0)
			break;
		i++;
	}

	/*init queue*/	
	memset(&myioque, 0, sizeof(struct myio_cir_que));	

	printk("my write function\n");
	return count;
}

static ssize_t my_read(struct file *f, char __user *userArray, size_t s, loff_t * l){

	if(s <= PROCSIZE){
        	memcpy(userArray, my_proc_buf, s);
		printk("my read\n");
		return s; 	
        }

	else {
		printk("error : userArray size is smaller than my_proc_buf's size\n");
		return -1;
	}
}


/*global variables*/
static struct proc_dir_entry *my_proc_dir;
static struct proc_dir_entry *my_proc_file;
//EXPORT_SYMBOL(my_proc_file);
static const struct file_operations myproc_fops = { 
		.owner = THIS_MODULE,
		.open = my_open, 
		.write = my_write,
		.read = my_read,
};
struct timespec my_bio_time;

static int __init init_my_module(void) {
	/*init circular queue*/
	memset(&myioque, 0, sizeof(struct myio_cir_que));	
	memset(my_proc_buf, 0, sizeof(my_proc_buf));

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
	struct myio_cir_que *que = &myioque;
	/*need routine for exception of print_que_to_proc failure*/
	if (que->que_count == QUESIZE) {
		if(my_write(NULL,NULL,0,NULL) < 0)
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

MODULE_AUTHOR("KIM");
MODULE_DESCRIPTION("FIRST MADE KERNEL MODULE");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");

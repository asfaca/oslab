#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/bio.h>

#define PROCSIZE 100000
#define ASCIENTER 0x0A
#define ASCISPACE 0x20

extern struct myio_cir_que myioque;
extern int my_notbooting;

static char my_proc_buf[PROCSIZE] = "";
int my_proc_fp = 0;

int my_open(struct inode *inode, struct file *file) {
	my_proc_fp = 0;
	printk("my open function\n");
	return 0;
}

ssize_t my_write(struct file *file, const char __user *user_buffer, 
		   size_t count, loff_t *ppos) {
	int i = 0;
	if (count > 0)
		return count;	
	
	if (my_proc_fp > PROCSIZE-100) 
		return count;

	if (myioque.que_count == QUESIZE) {
		i = myioque.curr_index;

		while(--myioque.que_count != 0){
			if (i == QUESIZE)
				i = 0;	
			my_proc_fp += sprintf(&my_proc_buf[my_proc_fp], "%s", myioque.que[i].name);
			my_proc_buf[my_proc_fp++] = ASCISPACE;
			my_proc_fp += sprintf(&my_proc_buf[my_proc_fp], "%lu", myioque.que[i].sector_num);
			my_proc_buf[my_proc_fp++] = ASCISPACE;
			my_proc_fp += sprintf(&my_proc_buf[my_proc_fp], "%ld", myioque.que[i].time.tv_sec);
			my_proc_buf[my_proc_fp++] = ASCIENTER;
			i++;
		}
		memset(&myioque, 0, sizeof(struct myio_cir_que));
	}

	printk("my write function end\n");
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

static int __init init_my_module(void) {
	int i = 100000;
	/*init proc buffer*/
	memset(my_proc_buf, 0, sizeof(my_proc_buf));
	my_notbooting = 1;
	/*create proc*/
	my_proc_dir = proc_mkdir("oslab_dir", NULL);
	my_proc_file = proc_create("oslab_file", 0600, my_proc_dir, &myproc_fops);
	
	while(i) {
		my_write(NULL, NULL, 0, NULL);
		udelay(100);
		i--;
	}

	printk(KERN_ALERT "module starts.\n");
	return 0;
}

static void __exit exit_my_module(void) {
	remove_proc_entry("oslab_file", my_proc_dir);
	remove_proc_entry("oslab_dir", NULL);
	printk(KERN_ALERT "module terminates\n");
	return;
}

module_init(init_my_module);
module_exit(exit_my_module);

MODULE_AUTHOR("KIM");
MODULE_DESCRIPTION("FIRST MADE KERNEL MODULE");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");

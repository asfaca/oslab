#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 100000

int main(void) {
	int fd = open("/proc/oslab_dir/oslab_file", O_RDONLY);
	char buf[SIZE] = "";

	if (read(fd, buf, SIZE) < 0) {
		printf("fail\n");
		return 0;
	}

	int fd_write = open("print-proc.txt", O_WRONLY);
	if (write(fd_write, buf, SIZE) < 0) {
		printf("write fail\n");
		return 0;
	}


	return 0;
}

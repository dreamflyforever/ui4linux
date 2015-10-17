#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "linux/fb.h"
#include <unistd.h> 
#include <sys/ioctl.h> 
#include "_lcd_show.h"

int main(int argc, char ** argv)
{
	int x_offset = 0;
	int y_offset = 0;

	if (argc != 4) {
		printf("usage : bmp_test test.bmp  x_offset, y_offset\n"); 
		return  -1;
	}

	printf("bmp file : %s\n", argv[1]); 
	x_offset = atoi(argv[2]);
	y_offset = atoi(argv[3]);

	if (x_offset < 0 || y_offset < 0) {
		printf("x_offset or y_offset must be >0\n");
		return (-1);
	}

	picture_write(x_offset, y_offset, argv[1]);

	return 0;
}

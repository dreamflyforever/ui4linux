#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <linux/fb.h>
#include <unistd.h> 
#include <sys/ioctl.h> 
#include "sys/mman.h"
#include "_lcd_show.h"

struct bmp_info{
	unsigned short bmp_type;
	unsigned int bmp_size;
	unsigned int reserve;
	unsigned int data_offset;
	unsigned int header_size;
	int width;
	int height;
	unsigned int planes;
	int bits_per_pixel;
	int compression;
	unsigned int bitmap_data_size;
	unsigned int h_resolution;
	unsigned int v_resolution;
	unsigned int color;
	unsigned int important_colors;
};

typedef struct bmp_info bmp_info;
unsigned short short_le(unsigned short *p)
{
	char *p1 = (char *)p;
	*p =  (*p1)<<8| *(p1+1);
	return *p1;
}

unsigned int int_le(unsigned int *p)
{
	unsigned char *p1 = (unsigned char *)p;

	*p= (*p1<<24)| (*(p1+1)<<16) |(*(p1+2)<<8)|(*(p1+3));
	return *p;
}

void prn_addr(bmp_info *p)
{
	printf("pbmpsize :%p\n",&(p->bmp_type));
	printf("pbmpsize :%p\n",&(p->bmp_size));
	printf("pbmpsize :%p\n",&(p->reserve));
	printf("pbmpsize :%p\n",&(p->data_offset));
	printf("pbmpsize :%p\n",&(p->header_size));
	printf("pbmpsize :%p\n",&(p->width));
	printf("pbmpsize :%p\n",&(p->height));
	printf("pbmpsize :%p\n",&(p->planes));
	printf("pbmpsize :%p\n",&(p->bits_per_pixel));
	printf("pbmpsize :%p\n",&(p->compression));
	printf("pbmpsize :%p\n",&(p->bitmap_data_size));
	printf("pbmpsize :%p\n",&(p->h_resolution));
	printf("pbmpsize :%p\n",&(p->v_resolution));

}
int read_bmp_info(int fd ,bmp_info *p)
{
	read(fd,&(p->bmp_type),sizeof(short));

	short_le(&(p->bmp_type));

	read(fd,&(p->bmp_size),sizeof(int));

	read(fd,&(p->reserve),sizeof(int));

	read(fd,&(p->data_offset),sizeof(int));

	read(fd,&(p->header_size),sizeof(int));

	read(fd,&(p->width),sizeof(p->width));

	read(fd,&(p->height),sizeof(p->height));

	read(fd,&(p->planes),2);

	read(fd,&(p->bits_per_pixel),2);

	read(fd,&(p->compression),4);

	read(fd,&(p->bitmap_data_size),4);

	read(fd,&(p->h_resolution),4);

	read(fd,&(p->v_resolution),4);

	read(fd,&(p->color),sizeof(p->color));

	read(fd,&(p->important_colors),sizeof(p->important_colors));

	return 0;

}



void prn_bmp_info(bmp_info *bmp)
{
	printf("bmp_type:0x%0x\nbmp_size:0x%x\nbmp_data_offset :0x%x\nbmp_header_size:0x%x\n",\
			bmp->bmp_type,bmp->bmp_size,bmp->data_offset,bmp->header_size);
	printf("bmp_width :0x%x\nbmp_height:0x%x\nbmp_planes:0x%x\nbmp_bits_per_pixel :0x%x\n",\
			bmp->width,bmp->height,bmp->planes,bmp->bits_per_pixel);
	printf("bmp_compression :0x%x\nbitmap_data_size:0x%x\nbmp_h:0x%x\nbmp_w:0x%x\nbmp_color:0x%x\n",\
			bmp->compression,bmp->bitmap_data_size,bmp->h_resolution,bmp->v_resolution,bmp->color);
	printf("bmp_important_colors:0x%x\n",bmp->important_colors);
}

int  get_bmp_data(struct fb_var_screeninfo *vinfo,
		unsigned  char *buf,
		char *file_name,
		int x_offset,
		int y_offset)
{
	int fd = 0 ;
	int i  = 0;
	int j  = 0;
	int k  = 0;
	int addr = 0; 
	unsigned short data = 0;
	unsigned char tmp[4] = {0};
	bmp_info bmp_info ;

	fd = open((const char *)file_name,O_RDONLY);
	if (fd<0) {
		perror("open");
		exit(-1);
	}

	memset(&bmp_info,0,sizeof(bmp_info));
	read_bmp_info(fd,&bmp_info);
	prn_bmp_info(&bmp_info);

	/*    if(bmp_info.bits_per_pixel !=24){
	      printf("bmp image  : %d must be 24bits\n",bmp_info.bits_per_pixel);
	      exit(-1);
	      }
	 */
	if((vinfo->bits_per_pixel != 32) && (vinfo->bits_per_pixel != 16)){
		printf("fb bits_per_pixel must be 32 or 16\n");
		exit(-1);
	}

	lseek(fd, bmp_info.data_offset, 0); 

	if ((bmp_info.height + y_offset) > vinfo->yres) {
		printf(" x_offset or  y_offset out xres or yres\n");
		return -1;

	}

	if (bmp_info.height <= 0) {

		for(i=0;i<bmp_info.height;i++){
			k=0;
			for(j=0;j<bmp_info.width*bmp_info.bits_per_pixel/8;j+=bmp_info.bits_per_pixel/8){
				addr=k+i*vinfo->xres*vinfo->bits_per_pixel/8; 
				k+=vinfo->bits_per_pixel/8;
				memset(tmp,0,bmp_info.bits_per_pixel/8);
				read(fd,tmp,bmp_info.bits_per_pixel/8);
				* (buf+addr)   = tmp[0];
				* (buf+addr+1) = tmp[1];
				* (buf+addr+2) = tmp[2]; 

				if(vinfo->bits_per_pixel == 16){
					data =(tmp[0]>>3) |((tmp[1]>>2)<<5) |((tmp[2]>>3)<<11);
					* (buf+addr) = data&0xff;
					* (buf+addr+1) = data>>8;
				}

				if(vinfo->bits_per_pixel == 24){
					* (buf+addr)   = tmp[0];
					* (buf+addr+1) = tmp[1];
					* (buf+addr+2) = tmp[2];

				} 

				if(vinfo->bits_per_pixel == 32){
					* (buf+addr)   = tmp[0];
					* (buf+addr+1) = tmp[1];
					* (buf+addr+2) = tmp[2];
					* (buf+addr+3) = 255;
				}
			}
		} 

	} else {
		for (i = bmp_info.height-1 + y_offset; i>=y_offset; i--) {
			k=x_offset;
			for (j = bmp_info.width*bmp_info.bits_per_pixel/8-1;j>=0; j -= bmp_info.bits_per_pixel/8){
				addr=k+i*vinfo->xres*vinfo->bits_per_pixel/8; 
				k+=vinfo->bits_per_pixel/8;
				memset(tmp,0,bmp_info.bits_per_pixel/8);
				read(fd,tmp,bmp_info.bits_per_pixel/8);

				* (buf+addr)   = tmp[0];
				* (buf+addr+1) = tmp[1];
				* (buf+addr+2) = tmp[2]; 

				if (vinfo->bits_per_pixel == 16) {
					data =(tmp[0]>>3) |((tmp[1]>>2)<<5) |((tmp[2]>>3)<<11);
					* (buf+addr) = data&0xff;
					* (buf+addr+1) = data>>8;
				}

				if(vinfo->bits_per_pixel == 24){

					* (buf+addr)   = tmp[0];
					* (buf+addr+1) = tmp[1];
					* (buf+addr+2) = tmp[2];
				} 

				if (vinfo->bits_per_pixel == 32) {
					* (buf+addr)   = tmp[0];
					* (buf+addr+1) = tmp[1];
					* (buf+addr+2) = tmp[2];
					* (buf+addr+3) = 255;
				}
			}
		}
	}
	return 0;
}

int picture_write(int x, int y, char *p)
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	unsigned long screensize = 0;
	unsigned char *fbp = NULL;
	char len = 0; 

	fbfd = open("/dev/fb0",O_RDWR);
	if(fbfd == -1){
		printf("Error : cannot open framebuffer device .\n");
		exit(-1);
	}
	printf(" /dev/fb0 opend\n");

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error get varible infor\n");
		exit(-1);
	}

	printf("xres :%d \nyres :%d\nbpp :%d \n",vinfo.xres,vinfo.yres,vinfo.bits_per_pixel);
	if (vinfo.bits_per_pixel != 32 && vinfo.bits_per_pixel != 16) {
		printf("Error ,not support this bits_per_pixel\n");
		exit(-1);
	}

	len = vinfo.bits_per_pixel/8;

	screensize = vinfo.xres * vinfo.yres * len;
	fbp = (unsigned char *)mmap(0,screensize,PROT_READ | PROT_WRITE,MAP_SHARED,fbfd,0);

	if ((int)fbp == -1){
		printf("Error failed mmap\n");
		exit(-1);
	}

	//memset(fbp, 0, screensize);
	//get_bmp_data(&vinfo, fbp, p, x, y);
	get_png_data(&vinfo, fbp, p, x, y);

	munmap(fbp, screensize);

	close(fbfd);

	return 0;
}

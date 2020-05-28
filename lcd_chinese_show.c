#include "_lcd_show.h"

void lcd_clear(struct ft *f)
{
	memset(f->fbmem, 0, f->screen_size);
}

/*color : 0x00RRGGBB*/
void lcd_put_pixel(struct ft *f, int x, int y, unsigned int color)
{
	unsigned char *pen_8 = f->fbmem + y*f->line_width + x*f->pixel_width;
	unsigned short *pen_16;

	unsigned int *pen_32;

	unsigned int red, green, blue;

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (f->var.bits_per_pixel) {
	case 8:
	{
		*pen_8 = color;
		break;
	}
	case 16:
	{
		/* 565 */
		red   = (color >> 16) & 0xff;
		green = (color >> 8) & 0xff;
		blue  = (color >> 0) & 0xff;
		color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
		*pen_16 = color;
		break;
	}
	case 32:
	{
		*pen_32 = color;
		break;
	}
	default:
	{
		printf("can't surport %dbpp\n", f->var.bits_per_pixel);
		break;
	}
	}
}


void lcd_put_ascii(struct ft *f, int x, int y, unsigned char c)
{
	unsigned char *dots = (unsigned char *)&fontdata_8x16[c*16];
	int i, b;
	unsigned char byte;

	/*8 * 16, display 16 line*/
	for (i = 0; i < 16; i++) {
		byte = dots[i];
		for (b = 7; b >= 0; b--) {
			if (byte & (1<<b)) {
				/* show */
				lcd_put_pixel(f, x + (7-b)*2, y+i*2, 0xffffff);
				lcd_put_pixel(f, x + (7-b) *2 + 1, y+i*2, 0xffffff);
			} else {
				/* hide */
				lcd_put_pixel(f, x + (7-b) *2 + 1, y+i *2, 0);
			}
		}
	}
}

void lcd_put_chinese(struct ft *f, int x, int y, unsigned char *str)
{
	unsigned int area  = str[0] - 0xA1;
	unsigned int where = str[1] - 0xA1;
	unsigned char *dots = f->font_mem + (area * 94 + where)*32;
	unsigned char byte;

	int i, j, b;
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 2; j++) {
			byte = dots[i*2 + j];
			for (b = 7; b >=0; b--) {
				if (byte & (1<<b)) {
					/* show */
					lcd_put_pixel(f, (7-b)*2 +x+j*16, y+i*2, 0xffffff);
					lcd_put_pixel(f, (7-b) *2 +1+x+j*16, y+i*2, 0xffffff);
				} else {
					/* hide */
					lcd_put_pixel(f, (7-b)*2+x+j*16, y+i*2, 0);
					lcd_put_pixel(f, (7-b)*2+1+x+j*16, y+i*2, 0);
				}
			}
		}
	}
}

int ft_init(struct ft *f)
{
	struct stat hzk_stat;

	f->fd_fb = open("/dev/fb0", O_RDWR);
	if (f->fd_fb < 0) {
		printf("can't open /dev/fb0\n");
		return -1;
	}

	if (ioctl(f->fd_fb, FBIOGET_VSCREENINFO, &f->var)) {
		printf("can't get f->var\n");
		return -1;
	}

	if (ioctl(f->fd_fb, FBIOGET_FSCREENINFO, &f->fix)) {
		printf("can't get f->fix\n");
		return -1;
	}

	f->line_width  = f->var.xres * f->var.bits_per_pixel / 8;
	f->pixel_width = f->var.bits_per_pixel / 8;
	f->screen_size = f->var.xres * f->var.yres * f->var.bits_per_pixel / 8;
	f->fbmem = (unsigned char *)mmap(NULL , f->screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, f->fd_fb, 0);
	if (f->fbmem == (unsigned char *)-1) {
		printf("can't mmap\n");
		return -1;
	}

	f->fd_font = open("/usr/share/vr/tone/HZK16", O_RDONLY);
	if (f->fd_font < 0) {
		printf("first time can't open HZK16\n");
		f->fd_font = open("/usr/data/HZK16", O_RDONLY);
		if (f->fd_font < 0) {
			printf("can't open HZK16\n");
		}
		return -1;
	}
	if(fstat(f->fd_font, &hzk_stat)) {
		printf("can't get fstat\n");
		return -1;
	}
	f->font_mem = (unsigned char *)mmap(NULL , hzk_stat.st_size, PROT_READ, MAP_SHARED, f->fd_font, 0);
	if (f->font_mem == (unsigned char *)-1) {
		printf("can't mmap for hzk16\n");
		return -1;
	}

	/*clear screen*/
	lcd_clear(f);
#if 0
	/*just test ascii*/
	int i;
	for (i = 33; i <= 126; i++) {
		lcd_put_ascii(f, i % 11 * 16, i/11 * 16, i);
	}
	sleep(5);
#endif
	lcd_clear(f);

	return 0;
}

void print_chinese(struct ft *f, unsigned char *chinese_str, int x, int y)
{
	if (f->fd_font < 0) {
		printf("print: can't open /dev/fb0\n");
		return ;
	}

	int distance = 35;
	int num_per_line = 7;
	int i;
	unsigned char tmp[2];
	int size = strlen((const char *)chinese_str);
	for (i = 0; i < size/2; i++) {
		tmp[0] = chinese_str[0+i*2];
		tmp[1] = chinese_str[1+i*2];
		lcd_put_chinese(f, x + i % num_per_line * distance, y + i/num_per_line * distance, tmp);
	}
}

void print_ascii(struct ft *f, unsigned char *ascii_str, int x, int y)
{
	if (f->fd_font < 0) {
		printf("print: can't open /dev/fb0\n");
		return ;
	}
	int i;
	int distance = 25;
	int num_per_line = 10;
	int size = strlen((const char *)ascii_str);
	for (i = 0; i < size; i++) {
		lcd_put_ascii(f, x + i % num_per_line * distance,
				y + i/num_per_line * distance, ascii_str[i]);
	}
}

void fb_close(struct ft *f)
{
	if (f->fd_font < 0) {
		printf("can't open /dev/fb0\n");
		return ;
	}
	close(f->fd_fb);
	close(f->fd_font);
}

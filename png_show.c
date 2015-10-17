#include <stdlib.h>
#include <png.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "_lcd_show.h"

#define width_mm 3672
#define hight_mm 4896

int  get_png_data(struct fb_var_screeninfo *vinfo,
		unsigned  char *buf,
		char *file_name,
		int x_offset,
		int y_offset)
{
	if ((x_offset < 400) || (x_offset >= width_mm) ||
		(y_offset < 400) || (y_offset > hight_mm)) {
		printf("make sure x,y >= 0, and x < %d, y < %d\n", width_mm, hight_mm);
		return 0;
	}

	float k  = 0;
	int addr = 0; 

	FILE* file = fopen(file_name, "rb");
	if (file == NULL) {
		printf(">>>>>>>>> %s open failed\n", file_name);
		return -1;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	png_infop info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, file);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	int m_width = png_get_image_width(png_ptr, info_ptr);
	int m_height = png_get_image_height(png_ptr, info_ptr);

	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

	int i, j;

	for (i = 0; i < m_height; i++) {
		k = (x_offset-300) / 15 * 4;
		for (j = 0; j < (4 * m_width); j += 4) {
			addr = k + vinfo->xres * (i + (y_offset - 300) / 15 - 1) * 4;
			k+=4;
			*(buf+addr)   = row_pointers[i][j + 2]; /*blue*/
			*(buf+addr+1) = row_pointers[i][j + 1]; /*green*/
			*(buf+addr+2) = row_pointers[i][j]; /*red*/
			*(buf+addr+3) = row_pointers[i][j + 3]; /*alpha*/
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(file);
	return 0;
}

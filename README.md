### ui4linux
ui4linux is easily interface for printing chinese and picture based on linux 
framebuffer.

### Compiling
* `./libcompile.sh` will product `liblcd.so`.
* `./compile_test_case.sh` will product `picture_show` and `chinese_show`.

### Using
* chinese show	

		int main(int argc, char **argv)
		{
			struct ft f;
			ft_init(&f);
			print_chinese(&f, "测试汉字", 60, 130);
			sleep(10);
			lcd_clear(&f);
			print_ascii(&f, "test alphabet and number", 60, 130);
			fb_close(&f);
			return 0; 
		}

* picture show

	picture_write(x_millimeter, y_millimeter, path_of_picture);

### Copyright
All copyrights are held by lapsule INC.  
Written by Jim Yang.

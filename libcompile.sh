mipsel-linux-gcc -fPIC --share bmp_show.c png_show.c lcd_chinese_show.c -I. -o liblcdshow.so -I. -I/home/jim/test/lcd/chinese_show/pro/libpng -Wall -L. -lpng -lz
mipsel-linux-strip liblcdshow.so

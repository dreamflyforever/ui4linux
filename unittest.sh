mipsel-linux-gcc picture_show.c -o picture_show -Werror -I. -L. -llcdshow -lpng -lz
mipsel-linux-gcc chinese_show.c -o chinese_show -finput-charset=UTF-8 -fexec-charset=GBK -Werror -I. -L. -llcdshow -llcdshow -lpng -lz
mipsel-linux-strip picture_show
mipsel-linux-strip chinese_show

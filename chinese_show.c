#include "_lcd_show.h"

/*-finput-charset=UTF-8 -fexec-charset=GBK*/
struct ft f;
int main(int argc, char **argv)
{
	ft_init(&f);
	print_chinese(&f, "蓝牙断开", 60, 130);

	fb_close(&f);
	return 0; 
}

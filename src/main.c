#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "epd_driver.h"
#include "epd_gpio.h"
#include "epd_graphics.h"
#include "epd_image_data.h"

void main(void)
{
	gpio_init(); // 初始化GPIO
	k_msleep(500);

	epd_hw_init(); // 初始化電子紙

	while (1)
	{
		printk("全螢幕刷新\n");
		epd_clear_frame();
		epd_display_frame_image(epd_bitmap_images_blackcat);
		epd_sleep();
		k_msleep(50);
		while (1)
		{
			k_msleep(1000);
		}
	}
}
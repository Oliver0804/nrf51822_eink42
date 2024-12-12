#include "epd_gpio.h"
#include "epd_config.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* The devicetree node identifier for the "led0" alias. */
#define RES DT_ALIAS(respin)       // res
#define EPD_CS DT_ALIAS(cspin)     // cs
#define EPD_DC DT_ALIAS(dcpin)     // dc
#define EPD_BUSY DT_ALIAS(busypin) // busy
#define EPD_BS DT_ALIAS(bspin)     // busy
                                   // #define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec edp_res_pin = GPIO_DT_SPEC_GET(RES, gpios);
const struct gpio_dt_spec edp_cs_pin = GPIO_DT_SPEC_GET(EPD_CS, gpios);
const struct gpio_dt_spec edp_dc_pin = GPIO_DT_SPEC_GET(EPD_DC, gpios);
const struct gpio_dt_spec edp_busy_pin = GPIO_DT_SPEC_GET(EPD_BUSY, gpios);
const struct gpio_dt_spec edp_bs_pin = GPIO_DT_SPEC_GET(EPD_BS, gpios);
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void gpio_init(void)
{
    if (!device_is_ready(edp_cs_pin.port) || !device_is_ready(edp_dc_pin.port) ||
        !device_is_ready(edp_res_pin.port) || !device_is_ready(edp_busy_pin.port))
    {
        printk("GPIO 初始化失敗！\n");
        return;
    }

    gpio_pin_configure_dt(&edp_cs_pin, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&edp_dc_pin, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&edp_res_pin, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&edp_busy_pin, GPIO_INPUT | GPIO_PULL_UP);
    gpio_pin_configure_dt(&edp_bs_pin, GPIO_OUTPUT_ACTIVE);

    gpio_pin_set_dt(&edp_bs_pin, 0);
    printk("GPIO 初始化成功！\n");
}
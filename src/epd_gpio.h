#ifndef EPD_GPIO_H
#define EPD_GPIO_H

#include <zephyr/drivers/gpio.h>

extern const struct gpio_dt_spec edp_cs_pin;
extern const struct gpio_dt_spec edp_dc_pin;
extern const struct gpio_dt_spec edp_res_pin;
extern const struct gpio_dt_spec edp_busy_pin;
extern const struct gpio_dt_spec edp_bs_pin;

void gpio_init(void);

#endif
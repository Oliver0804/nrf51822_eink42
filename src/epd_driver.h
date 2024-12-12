#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/types.h>

void epd_hw_init(void);
void epd_reset(void);
void epd_write_command(uint8_t cmd);
void epd_write_data(uint8_t data);
void epd_spi_write_byte(uint8_t data);
void epd_set_lut(void);
void epd_clear_frame(void);
void epd_display_frame_image(const uint8_t *frame_buffer);
void epd_sleep(void);

#endif
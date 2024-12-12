#ifndef EPD_GRAPHICS_H
#define EPD_GRAPHICS_H

#include <zephyr/types.h>

void epd_clear_frame(void);
void epd_display_frame_image(const uint8_t *frame_buffer);

#endif
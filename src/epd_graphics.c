#include "epd_driver.h"
#include "epd_graphics.h"
#include "epd_config.h"
int width = EPD_WIDTH;
int height = EPD_HEIGHT;
void epd_clear_frame(void)
{
    // 設置電子紙解析度
    epd_write_command(RESOLUTION_SETTING);
    epd_write_data(width >> 8);    // 寬度高位元組
    epd_write_data(width & 0xff);  // 寬度低位元組
    epd_write_data(height >> 8);   // 高度高位元組
    epd_write_data(height & 0xff); // 高度低位元組

    // 開始數據傳輸 (第一層影像資料)
    epd_write_command(DATA_START_TRANSMISSION_1);
    k_msleep(2); // 確保命令被執行

    // 傳輸全白資料
    for (int i = 0; i < width / 8 * height; i++)
    {
        epd_write_data(0xFF); // 0xFF 表示白色像素
    }

    k_msleep(2);

    // 開始數據傳輸 (第二層影像資料)
    epd_write_command(DATA_START_TRANSMISSION_2);
    k_msleep(2); // 確保命令被執行

    // 傳輸全白資料
    for (int i = 0; i < width / 8 * height; i++)
    {
        epd_write_data(0xFF); // 0xFF 表示白色像素
    }

    k_msleep(2); // 保證傳輸完成
}

void epd_display_frame_image(const uint8_t *frame_buffer)
{
    // 設置電子紙解析度
    epd_write_command(RESOLUTION_SETTING);
    epd_write_data(width >> 8);    // 寬度高位元組
    epd_write_data(width & 0xff);  // 寬度低位元組
    epd_write_data(height >> 8);   // 高度高位元組
    epd_write_data(height & 0xff); // 高度低位元組

    // 設置 VCOM DC 偏壓
    epd_write_command(VCM_DC_SETTING);
    epd_write_data(0x12); // 偏壓值，依據資料手冊設置

    // 設置 VCOM 與數據間隔控制
    epd_write_command(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_write_data(0x97); // 設置白邊框，根據硬件資料手冊設置

    // 傳輸數據 (第一層影像資料)
    epd_write_command(DATA_START_TRANSMISSION_1);
    for (int i = 0; i < width / 8 * height; i++)
    {
        epd_write_data(0xFF); // 0xFF 表示白色像素
    }
    k_msleep(2);

    // 傳輸數據 (第二層影像資料)
    epd_write_command(DATA_START_TRANSMISSION_2);
    for (int i = 0; i < width / 8 * height; i++)
    {
        // 從影像緩衝區讀取數據
        epd_write_data(frame_buffer[i]);
    }
    k_msleep(2);

    // 啟動顯示刷新
    epd_write_command(DISPLAY_REFRESH); // 0x12 啟動電子紙刷新
    k_msleep(100);                      // 等待刷新

    // 等待忙狀態結束
    epd_read_busy(); // 等待 BUSY 引腳空閒
}
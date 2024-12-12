#include "epd_driver.h"
#include "epd_gpio.h"
#include "epd_config.h"
#include "lut.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/spi.h>

const struct device *spi_dev = DEVICE_DT_GET(SPI_BUS);

const struct spi_config spi_cfg = {
    .frequency = 1000000U,
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER};

void epd_read_busy(void)
{
    printk("等待電子紙閒置中...\n");
    while (gpio_pin_get_dt(&edp_busy_pin) == 1)
    {
        k_msleep(10); // 監測忙碌腳位狀態
    }
    printk("電子紙進入閒置狀態\n");
}
void epd_set_lut(void)
{
    uint8_t count;

    // 設置 VCOM 查找表
    epd_write_command(LUT_FOR_VCOM);
    for (count = 0; count < 36; count++)
    {
        epd_write_data(lut_vcom0[count]);
    }

    // 設置 WW (白到白)
    epd_write_command(LUT_WHITE_TO_WHITE);
    for (count = 0; count < 36; count++)
    {
        epd_write_data(lut_ww[count]);
    }

    // 設置 BW (黑到白)
    epd_write_command(LUT_BLACK_TO_WHITE);
    for (count = 0; count < 36; count++)
    {
        epd_write_data(lut_bw[count]);
    }

    // 設置 WB (白到黑)
    epd_write_command(LUT_WHITE_TO_BLACK);
    for (count = 0; count < 36; count++)
    {
        epd_write_data(lut_wb[count]);
    }

    // 設置 BB (黑到黑)
    epd_write_command(LUT_BLACK_TO_BLACK);
    for (count = 0; count < 36; count++)
    {
        epd_write_data(lut_bb[count]);
    }

    printk("LUT 設置完成\n");
}
void epd_sleep(void)
{
    epd_write_command(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_write_data(0x17);              // border floating
    epd_write_command(VCM_DC_SETTING); // VCOM to 0V
    epd_write_command(PANEL_SETTING);
    k_msleep(100);

    epd_write_command(POWER_SETTING); // VG&VS to 0V fast
    epd_write_data(0x00);
    epd_write_data(0x00);
    epd_write_data(0x00);
    epd_write_data(0x00);
    epd_write_data(0x00);
    k_msleep(100);

    epd_write_command(POWER_OFF); // power off
    epd_read_busy();
    epd_write_command(DEEP_SLEEP); // deep sleep
    epd_write_data(0xA5);
}
void epd_hw_init(void)
{
    printk("EPD_HW_Init 開始初始化電子紙\n");

    // 設置重置引腳為低電位
    int ret = gpio_pin_set_dt(&edp_res_pin, 0);
    if (ret < 0)
    {
        printk("設定重置引腳失敗\n");
    }
    else
    {
        printk("重置引腳已設置為低電位\n");
    }

    // 執行電子紙重置流程
    epd_reset();

    // 功率設置命令 (Power Setting)
    epd_write_command(0x06);
    epd_write_data(0x17); // 擴展電壓設置
    epd_write_data(0x17); // 擴展電壓設置
    epd_write_data(0x17); // 擴展電壓設置

    // 電源升壓命令 (Power On)
    epd_write_command(0x04);

    // 讀取功率狀態 (Power Status)
    epd_write_command(0x17);
    epd_write_data(0x16);

    // 面板設置命令 (Panel Setting)
    epd_write_command(0x01);
    epd_write_data(0x03); // VDS_EN, VDG_EN 啟用
    epd_write_data(0x00); // VCOM_HV, VGHL_LV[1], VGHL_LV[0] 設置為默認值
    epd_write_data(0x2b); // VDH 電壓高位設置
    epd_write_data(0x2b); // VDL 電壓低位設置

    // 功率設置命令 (重新設置)
    epd_write_command(0x06);
    epd_write_data(0x17); // 擴展電壓設置
    epd_write_data(0x17); // 擴展電壓設置
    epd_write_data(0x17); // 擴展電壓設置

    // 電源升壓命令
    epd_write_command(0x04);
    epd_read_busy(); // 等待升壓完成

    // VCOM 設置命令
    epd_write_command(0x00);
    epd_write_data(0xbf); // KW-BF   KWR-AF  BWROTP 設置值

    // 顯示刷新頻率設置 (Data Mode)
    epd_write_command(0x30);
    epd_write_data(0x3c); // 頻率選擇: 100Hz 或更高

    // 分辨率設置命令 (Resolution Setting)
    epd_write_command(0x61);
    epd_write_data(0x01); // 寬度高位 (示例設置)
    epd_write_data(0x90); // 寬度低位 (128)
    epd_write_data(0x01); // 高度高位
    epd_write_data(0x2c); // 高度低位

    // VCOM 直流偏壓設置命令 (VCOM DC Setting)
    epd_write_command(0x82);
    epd_write_data(0x12); // 偏壓設置值

    // VCOM 與數據間隔設置命令 (VCOM and Data Interval Setting)
    epd_write_command(0X50);
    epd_write_data(0x97); // 邊框顏色設置為白色 (0x97 表示白邊框)

    // 設置查找表 (LUT)
    epd_set_lut();

    printk("電子紙初始化完成\n");
}

/* 電子紙重置邏輯，基於 Zephyr API */
void epd_reset(void)
{
    // 第一次重置
    gpio_pin_set_dt(&edp_res_pin, 0); // EPD_W21_RST_0
    k_msleep(2);
    gpio_pin_set_dt(&edp_res_pin, 1); // EPD_W21_RST_1
    k_msleep(20);

    // 第二次重置
    gpio_pin_set_dt(&edp_res_pin, 0); // EPD_W21_RST_0
    k_msleep(2);
    gpio_pin_set_dt(&edp_res_pin, 1); // EPD_W21_RST_1
    k_msleep(20);

    // 第三次重置
    gpio_pin_set_dt(&edp_res_pin, 0); // EPD_W21_RST_0
    k_msleep(2);
    gpio_pin_set_dt(&edp_res_pin, 1); // EPD_W21_RST_1
    k_msleep(20);

    // 確保重置腳位處於高電位
    gpio_pin_set_dt(&edp_res_pin, 1); // EPD_W21_RST_1
    printk("電子紙重置完成\n");
}

void epd_write_command(uint8_t cmd)
{
    gpio_pin_set_dt(&edp_cs_pin, 0);
    gpio_pin_set_dt(&edp_dc_pin, 0); // 命令模式
    epd_spi_write_byte(cmd);
    gpio_pin_set_dt(&edp_cs_pin, 1);
}

void epd_write_data(uint8_t data)
{
    gpio_pin_set_dt(&edp_cs_pin, 0);
    gpio_pin_set_dt(&edp_dc_pin, 1); // 資料模式
    epd_spi_write_byte(data);
    gpio_pin_set_dt(&edp_cs_pin, 1);
}

void epd_spi_write_byte(uint8_t data)
{
    struct spi_buf tx_buf = {.buf = &data, .len = 1};
    struct spi_buf_set tx = {.buffers = &tx_buf, .count = 1};
    spi_write(spi_dev, &spi_cfg, &tx);
}
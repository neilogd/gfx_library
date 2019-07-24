#pragma once

#include "platform.h"

enum class ST7735Command : uint8_t
{
    NOP        = 0x00,
    SWRESET    = 0x01,
    RDDID      = 0x04,
    RDDST      = 0x09,
    SLPIN      = 0x10,
    SLPOUT     = 0x11,
    PTLON      = 0x12,
    NORON      = 0x13,
    INVOFF     = 0x20,
    INVON      = 0x21,
    DISPOFF    = 0x28,
    DISPON     = 0x29,
    CASET      = 0x2A,
    RASET      = 0x2B,
    RAMWR      = 0x2C,
    RAMRD      = 0x2E,
    PTLAR      = 0x30,
    COLMOD     = 0x3A,
    MADCTL     = 0x36,
    RDID1      = 0xDA,
    RDID2      = 0xDB,
    RDID3      = 0xDC,
    RDID4      = 0xDD,

    // 
    DELAY      = 0x80,
};

enum class ST7735MADCTLFlags
{
    MADCTL_MY  = 0x80,
    MADCTL_MX  = 0x40,
    MADCTL_MV  = 0x20,
    MADCTL_ML  = 0x10,
    MADCTL_RGB = 0x00,
};

static const uint16_t LCD_CS = GPIO_PIN_13;
static const uint16_t LCD_RST = GPIO_PIN_14;
static const uint16_t LCD_DC = GPIO_PIN_15;

static const uint16_t SPI2_MOSI = GPIO_PIN_15;
static const uint16_t SPI2_MISO = GPIO_PIN_14;
static const uint16_t SPI2_SCLK = GPIO_PIN_13;
static const uint16_t SPI2_NSS = GPIO_PIN_12;

class DisplayST7735
{
public:
    DisplayST7735();
    ~DisplayST7735();

    void init();

    void begin()
    {
        HAL_GPIO_WritePin(GPIOC, LCD_CS, GPIO_PIN_RESET);
    }

    void end()
    {
        HAL_GPIO_WritePin(GPIOC, LCD_CS, GPIO_PIN_SET);
    }

    void setAddrWindow(int16_t x, int16_t y, int16_t w, int16_t h);

    inline void writePixels(const uint16_t* pixels, int32_t numPixels)
    {
        HAL_SPI_Transmit(&hspi_, (uint8_t*)&pixels[0], numPixels * 2, 100);
        //HAL_SPI_Transmit_DMA(&hspi_, (uint8_t*)&pixels[0], numPixels * 2);
        //while(HAL_SPI_GetState(&hspi_) == HAL_SPI_STATE_BUSY_TX);

    }

private:
    inline void sendCommand(SPI_HandleTypeDef* hspi, uint8_t cmd, const uint8_t* addr, uint8_t num)
    {
        HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_RESET);
        HAL_SPI_Transmit(hspi, &cmd, 1, 100);
        HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_SET);
        HAL_SPI_Transmit(hspi, const_cast<uint8_t*>(addr), num, 100);
    }

    inline void writeCommand(SPI_HandleTypeDef* hspi, uint8_t cmd)
    {
        HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_RESET);
        HAL_SPI_Transmit(hspi, &cmd, 1, 100);
        HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_SET);
    }

    DMA_HandleTypeDef hdma_;
    SPI_HandleTypeDef hspi_;
    int16_t colstart_;
    int16_t rowstart_;
};

#pragma once

#include "platform.h"

struct DisplayConfig
{
    int16_t colStart = 0;
    int16_t rowStart = 0;

#if defined(PLATFORM_STM32)
    SPI_HandleTypeDef* hspi = nullptr;
    DMA_HandleTypeDef* hdma = nullptr;
#endif

};

class DisplayST7735
{
public:
    DisplayST7735();
    ~DisplayST7735();

    void init(const DisplayConfig& config);

    void begin();
    void end();
    void setAddrWindow(int16_t x, int16_t y, int16_t w, int16_t h);
    void writePixels(const uint16_t* pixels, int32_t numPixels);
private:
    void waitForReady();
    void sendCommand(uint8_t cmd, const uint8_t* addr, uint8_t num);
    void writeCommand(uint8_t cmd);
    void writeCommandStream(const uint8_t *addr);

#if defined(PLATFORM_STM32)
    DMA_HandleTypeDef* hdma_;
    SPI_HandleTypeDef* hspi_;
#endif
    int16_t colStart_;
    int16_t rowStart_;
};

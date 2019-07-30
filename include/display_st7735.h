#pragma once

#include "platform.h"

class DisplayST7735
{
public:
    DisplayST7735();
    ~DisplayST7735();

    void init();

    void begin();
    void end();
    void setAddrWindow(int16_t x, int16_t y, int16_t w, int16_t h);
    void writePixels(const uint16_t* pixels, int32_t numPixels);

private:
    void sendCommand(SPI_HandleTypeDef* hspi, uint8_t cmd, const uint8_t* addr, uint8_t num);
    void writeCommand(SPI_HandleTypeDef* hspi, uint8_t cmd);
    void writeCommandStream(SPI_HandleTypeDef* hspi, const uint8_t *addr);

    DMA_HandleTypeDef hdma_;
    SPI_HandleTypeDef hspi_;
    int16_t colstart_;
    int16_t rowstart_;
};

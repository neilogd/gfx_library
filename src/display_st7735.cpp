#include "display_st7735.h"

#include <memory.h>

namespace
{
    enum
    {
        ST_CMD_DELAY = 0x80,        // special signifier for command lists

        ST77XX_NOP = 0x00,
        ST77XX_SWRESET = 0x01,
        ST77XX_RDDID = 0x04,
        ST77XX_RDDST = 0x09,

        ST77XX_SLPIN = 0x10,
        ST77XX_SLPOUT = 0x11,
        ST77XX_PTLON = 0x12,
        ST77XX_NORON = 0x13,

        ST77XX_INVOFF = 0x20,
        ST77XX_INVON = 0x21,
        ST77XX_DISPOFF = 0x28,
        ST77XX_DISPON = 0x29,
        ST77XX_CASET = 0x2A,
        ST77XX_RASET = 0x2B,
        ST77XX_RAMWR = 0x2C,
        ST77XX_RAMRD = 0x2E,

        ST77XX_PTLAR = 0x30,
        ST77XX_COLMOD = 0x3A,
        ST77XX_MADCTL = 0x36,

        ST77XX_MADCTL_MY    = 0x80,
        ST77XX_MADCTL_MX = 0x40,
        ST77XX_MADCTL_MV = 0x20,
        ST77XX_MADCTL_ML = 0x10,
        ST77XX_MADCTL_RGB = 0x00,

        ST77XX_RDID1 = 0xDA,
        ST77XX_RDID2 = 0xDB,
        ST77XX_RDID3 = 0xDC,
        ST77XX_RDID4 = 0xDD,

        // Some register settings
        ST7735_MADCTL_BGR = 0x08,
        ST7735_MADCTL_MH = 0x04,

        ST7735_FRMCTR1 = 0xB1,
        ST7735_FRMCTR2 = 0xB2,
        ST7735_FRMCTR3 = 0xB3,
        ST7735_INVCTR = 0xB4,
        ST7735_DISSET5 = 0xB6,

        ST7735_PWCTR1 = 0xC0,
        ST7735_PWCTR2 = 0xC1,
        ST7735_PWCTR3 = 0xC2,
        ST7735_PWCTR4 = 0xC3,
        ST7735_PWCTR5 = 0xC4,
        ST7735_VMCTR1 = 0xC5,

        ST7735_PWCTR6 = 0xFC,

        ST7735_GMCTRP1 = 0xE0,
        ST7735_GMCTRN1 = 0xE1,
    };

    static const uint8_t
    Bcmd[] = {                                                      // Init commands for 7735B screens
        18,                                                         // 18 commands in list:
        ST77XX_SWRESET,     ST_CMD_DELAY,                           //    1: Software reset, no args, w/delay
            50,                                                     //         50 ms delay
        ST77XX_SLPOUT,        ST_CMD_DELAY,                         //    2: Out of sleep mode, no args, w/delay
            255,                                                    //         255 = max (500 ms) delay
        ST77XX_COLMOD,    1+ST_CMD_DELAY,                           //    3: Set color mode, 1 arg + delay:
            0x05,                                                   //         16-bit color
            10,                                                     //         10 ms delay
        ST7735_FRMCTR1, 3+ST_CMD_DELAY,                             //    4: Frame rate control, 3 args + delay:
            0x00,                                                   //         fastest refresh
            0x06,                                                   //         6 lines front porch
            0x03,                                                   //         3 lines back porch
            10,                                                     //         10 ms delay
        ST77XX_MADCTL,    1,                                        //    5: Mem access ctl (directions), 1 arg:
            0x08,                                                   //         Row/col addr, bottom-top refresh
        ST7735_DISSET5, 2,                                          //    6: Display settings #5, 2 args:
            0x15,                                                   //         1 clk cycle nonoverlap, 2 cycle gate
                                                                    //         rise, 3 cycle osc equalize
            0x02,                                                   //         Fix on VTL
        ST7735_INVCTR,    1,                                        //    7: Display inversion control, 1 arg:
            0x0,                                                    //         Line inversion
        ST7735_PWCTR1,    2+ST_CMD_DELAY,                           //    8: Power control, 2 args + delay:
            0x02,                                                   //         GVDD = 4.7V
            0x70,                                                   //         1.0uA
            10,                                                     //         10 ms delay
        ST7735_PWCTR2,    1,                                        //    9: Power control, 1 arg, no delay:
            0x05,                                                   //         VGH = 14.7V, VGL = -7.35V
        ST7735_PWCTR3,    2,                                        // 10: Power control, 2 args, no delay:
            0x01,                                                   //         Opamp current small
            0x02,                                                   //         Boost frequency
        ST7735_VMCTR1,    2+ST_CMD_DELAY,                           // 11: Power control, 2 args + delay:
            0x3C,                                                   //         VCOMH = 4V
            0x38,                                                   //         VCOML = -1.1V
            10,                                                     //         10 ms delay
        ST7735_PWCTR6,    2,                                        // 12: Power control, 2 args, no delay:
            0x11, 0x15,
        ST7735_GMCTRP1,16,                                          // 13: Gamma Adjustments (pos. polarity), 16 args + delay:
            0x09, 0x16, 0x09, 0x20,                                 //         (Not entirely necessary, but provides
            0x21, 0x1B, 0x13, 0x19,                                 //            accurate colors)
            0x17, 0x15, 0x1E, 0x2B,
            0x04, 0x05, 0x02, 0x0E,
        ST7735_GMCTRN1,16+ST_CMD_DELAY,                             // 14: Gamma Adjustments (neg. polarity), 16 args + delay:
            0x0B, 0x14, 0x08, 0x1E,                                 //         (Not entirely necessary, but provides
            0x22, 0x1D, 0x18, 0x1E,                                 //            accurate colors)
            0x1B, 0x1A, 0x24, 0x2B,
            0x06, 0x06, 0x02, 0x0F,
            10,                                                     //         10 ms delay
        ST77XX_CASET,     4,                                        // 15: Column addr set, 4 args, no delay:
            0x00, 0x02,                                             //         XSTART = 2
            0x00, 0x81,                                             //         XEND = 129
        ST77XX_RASET,     4,                                        // 16: Row addr set, 4 args, no delay:
            0x00, 0x02,                                             //         XSTART = 1
            0x00, 0x81,                                             //         XEND = 160
        ST77XX_NORON,         ST_CMD_DELAY,                         // 17: Normal display on, no args, w/delay
            10,                                                     //         10 ms delay
        ST77XX_DISPON,        ST_CMD_DELAY,                         // 18: Main screen turn on, no args, delay
            255 },                                                  //         255 = max (500 ms) delay

    Rcmd1[] = {                                                     // 7735R init, part 1 (red or green tab)
        15,                                                         // 15 commands in list:
        ST77XX_SWRESET,     ST_CMD_DELAY,                           //    1: Software reset, 0 args, w/delay
            150,                                                    //         150 ms delay
        ST77XX_SLPOUT,        ST_CMD_DELAY,                         //    2: Out of sleep mode, 0 args, w/delay
            255,                                                    //         500 ms delay
        ST7735_FRMCTR1, 3,                                          //    3: Framerate ctrl - normal mode, 3 arg:
            0x01, 0x2C, 0x2D,                                       //         Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR2, 3,                                          //    4: Framerate ctrl - idle mode, 3 args:
            0x01, 0x2C, 0x2D,                                       //         Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR3, 6,                                          //    5: Framerate - partial mode, 6 args:
            0x01, 0x2C, 0x2D,                                       //         Dot inversion mode
            0x01, 0x2C, 0x2D,                                       //         Line inversion mode
        ST7735_INVCTR,    1,                                        //    6: Display inversion ctrl, 1 arg:
            0x07,                                                   //         No inversion
        ST7735_PWCTR1,    3,                                        //    7: Power control, 3 args, no delay:
            0xA2,
            0x02,                                                   //         -4.6V
            0x84,                                                   //         AUTO mode
        ST7735_PWCTR2,    1,                                        //    8: Power control, 1 arg, no delay:
            0xC5,                                                   //         VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
        ST7735_PWCTR3,    2,                                        //    9: Power control, 2 args, no delay:
            0x0A,                                                   //         Opamp current small
            0x00,                                                   //         Boost frequency
        ST7735_PWCTR4,    2,                                        // 10: Power control, 2 args, no delay:
            0x8A,                                                   //         BCLK/2,
            0x2A,                                                   //         opamp current small & medium low
        ST7735_PWCTR5,    2,                                        // 11: Power control, 2 args, no delay:
            0x8A, 0xEE,
        ST7735_VMCTR1,    1,                                        // 12: Power control, 1 arg, no delay:
            0x0E,
        ST77XX_INVOFF,    0,                                        // 13: Don't invert display, no args
        ST77XX_MADCTL,    1,                                        // 14: Mem access ctl (directions), 1 arg:
            0xC8,                                                   //         row/col addr, bottom-top refresh
        ST77XX_COLMOD,    1,                                        // 15: set color mode, 1 arg, no delay:
            0x05 },                                                 //         16-bit color

    Rcmd2green[] = {                                                // 7735R init, part 2 (green tab only)
        2,                                                          //    2 commands in list:
        ST77XX_CASET,     4,                                        //    1: Column addr set, 4 args, no delay:
            0x00, 0x02,                                             //         XSTART = 0
            0x00, 0x7F+0x02,                                        //         XEND = 127
        ST77XX_RASET,     4,                                        //    2: Row addr set, 4 args, no delay:
            0x00, 0x01,                                             //         XSTART = 0
            0x00, 0x9F+0x01 },                                      //         XEND = 159

    Rcmd2red[] = {                                                  // 7735R init, part 2 (red tab only)
        2,                                                          //    2 commands in list:
        ST77XX_CASET,     4,                                        //    1: Column addr set, 4 args, no delay:
            0x00, 0x00,                                             //         XSTART = 0
            0x00, 0x7F,                                             //         XEND = 127
        ST77XX_RASET,     4,                                        //    2: Row addr set, 4 args, no delay:
            0x00, 0x00,                                             //         XSTART = 0
            0x00, 0x9F },                                           //         XEND = 159

    Rcmd2green144[] = {                                             // 7735R init, part 2 (green 1.44 tab)
        2,                                                          //    2 commands in list:
        ST77XX_CASET,     4,                                        //    1: Column addr set, 4 args, no delay:
            0x00, 0x00,                                             //         XSTART = 0
            0x00, 0x7F,                                             //         XEND = 127
        ST77XX_RASET,     4,                                        //    2: Row addr set, 4 args, no delay:
            0x00, 0x00,                                             //         XSTART = 0
            0x00, 0x7F },                                           //         XEND = 127

    Rcmd2green160x80[] = {                                          // 7735R init, part 2 (mini 160x80)
        2,                                                          //    2 commands in list:
        ST77XX_CASET,     4,                                        //    1: Column addr set, 4 args, no delay:
            0x00, 0x00,                                             //         XSTART = 0
            0x00, 0x4F,                                             //         XEND = 79
        ST77XX_RASET,     4,                                        //    2: Row addr set, 4 args, no delay:
            0x00, 0x00,                                             //         XSTART = 0
            0x00, 0x9F },                                           //         XEND = 159

    Rcmd3[] = {                                                     // 7735R init, part 3 (red or green tab)
        4,                                                          //    4 commands in list:
        ST7735_GMCTRP1, 16            ,                             //    1: Gamma Adjustments (pos. polarity), 16 args + delay:
            0x02, 0x1c, 0x07, 0x12,                                 //         (Not entirely necessary, but provides
            0x37, 0x32, 0x29, 0x2d,                                 //            accurate colors)
            0x29, 0x25, 0x2B, 0x39,
            0x00, 0x01, 0x03, 0x10,
        ST7735_GMCTRN1, 16            ,                             //    2: Gamma Adjustments (neg. polarity), 16 args + delay:
            0x03, 0x1d, 0x07, 0x06,                                 //         (Not entirely necessary, but provides
            0x2E, 0x2C, 0x29, 0x2D,                                 //            accurate colors)
            0x2E, 0x2E, 0x37, 0x3F,
            0x00, 0x00, 0x02, 0x10,
        ST77XX_NORON,         ST_CMD_DELAY,                         //    3: Normal display on, no args, w/delay
            10,                                                     //         10 ms delay
        ST77XX_DISPON,        ST_CMD_DELAY,                         //    4: Main screen turn on, no args w/delay
            100                                                     //         100 ms delay
    };    

#if defined(PLATFORM_STM32)
    static GPIO_TypeDef* LCD_GPIO = GPIOC;
    static GPIO_TypeDef* SPI2_GPIO = GPIOB;

    static const uint16_t LCD_CS = GPIO_PIN_13;
    static const uint16_t LCD_RST = GPIO_PIN_14;
    static const uint16_t LCD_DC = GPIO_PIN_15;

    static const uint16_t SPI2_MOSI = GPIO_PIN_15;
    static const uint16_t SPI2_MISO = GPIO_PIN_14;
    static const uint16_t SPI2_SCLK = GPIO_PIN_13;
    static const uint16_t SPI2_NSS = GPIO_PIN_12;
#endif // defined(PLATFORM_STM32)

#if defined(PLATFORM_RPI_RP2)
    static const uint16_t LCD_CS = 5;
    static const uint16_t LCD_RST = 6;
    static const uint16_t LCD_DC = 7;

    static const uint16_t SPI2_MOSI = 3;
    static const uint16_t SPI2_MISO = 4;
    static const uint16_t SPI2_SCLK = 2;
    static const uint16_t SPI2_NSS = 5;

    #define SPI_PORT spi0
#endif // defined(PLATFORM_RPI_RP2)
}

DisplayST7735::DisplayST7735()
{

}

DisplayST7735::~DisplayST7735()
{
}

void DisplayST7735::init(const DisplayConfig& config)
{
    colStart_ = config.colStart;
    rowStart_ = config.rowStart;
#if defined(PLATFORM_STM32)
    hspi_ = config.hspi;
    hdma_ = config.hdma;

    // Do reset.
    HAL_GPIO_WritePin(LCD_GPIO, LCD_RST, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_GPIO, LCD_RST, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO, LCD_RST, GPIO_PIN_SET);
#endif

    begin();
    writeCommandStream(Rcmd1);

#if defined(PLATFORM_STM32)
    HAL_GPIO_WritePin(LCD_GPIO, LCD_CS, GPIO_PIN_RESET);
#elif defined(PLATFORM_RPI_RP2)
    gpio_put(LCD_CS, 0);
#endif

    uint8_t madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    sendCommand(ST77XX_MADCTL, &madctl, 1);

    // Clear to black.
    uint16_t black[32];
    memset(&black, 0, sizeof(black));

    setAddrWindow(0, 0, 128, 128);
    for(int i = 0; i < (128*128/32); ++i)
        writePixels(black, 32);

    // Turn on display.
    writeCommandStream(Rcmd3);

    end();
}

void DisplayST7735::begin()
{
#if defined(PLATFORM_STM32)
    HAL_GPIO_WritePin(LCD_GPIO, LCD_CS, GPIO_PIN_RESET);
#elif defined(PLATFORM_RPI_RP2)
    gpio_put(LCD_CS, 0);
#endif
}

void DisplayST7735::end()
{
#if defined(PLATFORM_STM32)
    HAL_GPIO_WritePin(LCD_GPIO, LCD_CS, GPIO_PIN_SET);
#elif defined(PLATFORM_RPI_RP2)
    gpio_put(LCD_CS, 1);
#endif
}

void DisplayST7735::setAddrWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    x += colStart_;
    y += rowStart_;
    uint32_t xa = ((uint32_t)x << 16) | (x+w-1);
    uint32_t ya = ((uint32_t)y << 16) | (y+h-1);
    uint8_t* pxa = (uint8_t*)&xa;
    uint8_t* pya = (uint8_t*)&ya;

#if defined(PLATFORM_STM32)
    writeCommand(ST77XX_CASET);
    HAL_SPI_Transmit(hspi_, &pxa[3], 1, 100);
    HAL_SPI_Transmit(hspi_, &pxa[2], 1, 100);
    HAL_SPI_Transmit(hspi_, &pxa[1], 1, 100);
    HAL_SPI_Transmit(hspi_, &pxa[0], 1, 100);

    writeCommand(ST77XX_RASET);
    HAL_SPI_Transmit(hspi_, &pya[3], 1, 100);
    HAL_SPI_Transmit(hspi_, &pya[2], 1, 100);
    HAL_SPI_Transmit(hspi_, &pya[1], 1, 100);
    HAL_SPI_Transmit(hspi_, &pya[0], 1, 100);

    writeCommand(ST77XX_RAMWR); // write to RAM
#elif defined(PLATFORM_RPI_RP2)
    writeCommand(ST77XX_CASET);
    spi_write_blocking(SPI_PORT, &pxa[3], 1);
    spi_write_blocking(SPI_PORT, &pxa[2], 1);
    spi_write_blocking(SPI_PORT, &pxa[1], 1);
    spi_write_blocking(SPI_PORT, &pxa[0], 1);

    writeCommand(ST77XX_RASET);
    spi_write_blocking(SPI_PORT, &pya[3], 1);
    spi_write_blocking(SPI_PORT, &pya[2], 1);
    spi_write_blocking(SPI_PORT, &pya[1], 1);
    spi_write_blocking(SPI_PORT, &pya[0], 1);

    writeCommand(ST77XX_RAMWR); // write to RAM
#endif
}

void DisplayST7735::writePixels(const uint16_t* pixels, int32_t numPixels)
{
    waitForReady();
#if defined(PLATFORM_STM32)
    if(hdma_)
        HAL_SPI_Transmit_DMA(hspi_, (uint8_t*)&pixels[0], numPixels * 2);
    else
        HAL_SPI_Transmit(hspi_, (uint8_t*)&pixels[0], numPixels * 2, 100);
#elif defined(PLATFORM_RPI_RP2)
    spi_write_blocking(SPI_PORT, (uint8_t*)&pixels[0], numPixels * 2);
#endif
    // TODO: Remove!
    waitForReady();
}

void DisplayST7735::waitForReady()
{
#if defined(PLATFORM_STM32)
    while(HAL_SPI_GetState(hspi_) != HAL_SPI_STATE_READY);
#endif
}

void DisplayST7735::sendCommand(uint8_t cmd, const uint8_t* addr, uint8_t num)
{
    waitForReady();
#if defined(PLATFORM_STM32)
    HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi_, &cmd, 1, 100);
    HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_SET);
    HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(addr), num, 100);
#elif defined(PLATFORM_RPI_RP2)
    gpio_put(LCD_DC, 0);
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(LCD_DC, 1);
    spi_write_blocking(SPI_PORT, const_cast<uint8_t*>(addr), num);
#endif
}

void DisplayST7735::writeCommand(uint8_t cmd)
{
    waitForReady();
#if defined(PLATFORM_STM32)
    HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi_, &cmd, 1, 100);
    HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_SET);
#elif defined(PLATFORM_RPI_RP2)
    gpio_put(LCD_DC, 0);
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(LCD_DC, 1);
#endif
}

// Adafruit st7735 lib.
// https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST77xx.cpp
void DisplayST7735::writeCommandStream(const uint8_t *addr)
{
    uint8_t    numCommands, cmd, numArgs;
    uint16_t ms;
    numCommands = *addr++;
    while(numCommands--)
    {
        cmd = *addr++;
        numArgs    = *addr++;
        ms             = numArgs & ST_CMD_DELAY;
        numArgs &= ~ST_CMD_DELAY;
        sendCommand(cmd, addr, numArgs);
        addr += numArgs;

        if(ms)
        {
            ms = *addr++;
            if(ms == 255) ms = 500;
#if defined(PLATFORM_STM32)
            HAL_Delay(ms);
#elif defined(PLATFORM_RPI_RP2)
            sleep_ms(ms);
#endif
        }
    }
}

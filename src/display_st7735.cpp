#if !defined(PLATFORM_PC)
#include "display_st7735.h"

#define ST_CMD_DELAY      0x80    // special signifier for command lists

#define ST77XX_NOP        0x00
#define ST77XX_SWRESET    0x01
#define ST77XX_RDDID      0x04
#define ST77XX_RDDST      0x09

#define ST77XX_SLPIN      0x10
#define ST77XX_SLPOUT     0x11
#define ST77XX_PTLON      0x12
#define ST77XX_NORON      0x13

#define ST77XX_INVOFF     0x20
#define ST77XX_INVON      0x21
#define ST77XX_DISPOFF    0x28
#define ST77XX_DISPON     0x29
#define ST77XX_CASET      0x2A
#define ST77XX_RASET      0x2B
#define ST77XX_RAMWR      0x2C
#define ST77XX_RAMRD      0x2E

#define ST77XX_PTLAR      0x30
#define ST77XX_COLMOD     0x3A
#define ST77XX_MADCTL     0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1      0xDA
#define ST77XX_RDID2      0xDB
#define ST77XX_RDID3      0xDC
#define ST77XX_RDID4 0xDD

// Some register settings
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_FRMCTR1    0xB1
#define ST7735_FRMCTR2    0xB2
#define ST7735_FRMCTR3    0xB3
#define ST7735_INVCTR     0xB4
#define ST7735_DISSET5    0xB6

#define ST7735_PWCTR1     0xC0
#define ST7735_PWCTR2     0xC1
#define ST7735_PWCTR3     0xC2
#define ST7735_PWCTR4     0xC3
#define ST7735_PWCTR5     0xC4
#define ST7735_VMCTR1     0xC5

#define ST7735_PWCTR6     0xFC

#define ST7735_GMCTRP1    0xE0
#define ST7735_GMCTRN1 0xE1

#define PROGMEM
namespace
{
    static const uint8_t PROGMEM
  Bcmd[] = {                        // Init commands for 7735B screens
    18,                             // 18 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      50,                           //     50 ms delay
    ST77XX_SLPOUT,    ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      255,                          //     255 = max (500 ms) delay
    ST77XX_COLMOD,  1+ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x05,                         //     16-bit color
      10,                           //     10 ms delay
    ST7735_FRMCTR1, 3+ST_CMD_DELAY, //  4: Frame rate control, 3 args + delay:
      0x00,                         //     fastest refresh
      0x06,                         //     6 lines front porch
      0x03,                         //     3 lines back porch
      10,                           //     10 ms delay
    ST77XX_MADCTL,  1,              //  5: Mem access ctl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST7735_DISSET5, 2,              //  6: Display settings #5, 2 args:
      0x15,                         //     1 clk cycle nonoverlap, 2 cycle gate
                                    //     rise, 3 cycle osc equalize
      0x02,                         //     Fix on VTL
    ST7735_INVCTR,  1,              //  7: Display inversion control, 1 arg:
      0x0,                          //     Line inversion
    ST7735_PWCTR1,  2+ST_CMD_DELAY, //  8: Power control, 2 args + delay:
      0x02,                         //     GVDD = 4.7V
      0x70,                         //     1.0uA
      10,                           //     10 ms delay
    ST7735_PWCTR2,  1,              //  9: Power control, 1 arg, no delay:
      0x05,                         //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3,  2,              // 10: Power control, 2 args, no delay:
      0x01,                         //     Opamp current small
      0x02,                         //     Boost frequency
    ST7735_VMCTR1,  2+ST_CMD_DELAY, // 11: Power control, 2 args + delay:
      0x3C,                         //     VCOMH = 4V
      0x38,                         //     VCOML = -1.1V
      10,                           //     10 ms delay
    ST7735_PWCTR6,  2,              // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16,              // 13: Gamma Adjustments (pos. polarity), 16 args + delay:
      0x09, 0x16, 0x09, 0x20,       //     (Not entirely necessary, but provides
      0x21, 0x1B, 0x13, 0x19,       //      accurate colors)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+ST_CMD_DELAY, // 14: Gamma Adjustments (neg. polarity), 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E,       //     (Not entirely necessary, but provides
      0x22, 0x1D, 0x18, 0x1E,       //      accurate colors)
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                           //     10 ms delay
    ST77XX_CASET,   4,              // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,                   //     XSTART = 2
      0x00, 0x81,                   //     XEND = 129
    ST77XX_RASET,   4,              // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,                   //     XSTART = 1
      0x00, 0x81,                   //     XEND = 160
    ST77XX_NORON,     ST_CMD_DELAY, // 17: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON,    ST_CMD_DELAY, // 18: Main screen turn on, no args, delay
      255 },                        //     255 = max (500 ms) delay

  Rcmd1[] = {                       // 7735R init, part 1 (red or green tab)
    15,                             // 15 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, 0 args, w/delay
      150,                          //     150 ms delay
    ST77XX_SLPOUT,    ST_CMD_DELAY, //  2: Out of sleep mode, 0 args, w/delay
      255,                          //     500 ms delay
    ST7735_FRMCTR1, 3,              //  3: Framerate ctrl - normal mode, 3 arg:
      0x01, 0x2C, 0x2D,             //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3,              //  4: Framerate ctrl - idle mode, 3 args:
      0x01, 0x2C, 0x2D,             //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6,              //  5: Framerate - partial mode, 6 args:
      0x01, 0x2C, 0x2D,             //     Dot inversion mode
      0x01, 0x2C, 0x2D,             //     Line inversion mode
    ST7735_INVCTR,  1,              //  6: Display inversion ctrl, 1 arg:
      0x07,                         //     No inversion
    ST7735_PWCTR1,  3,              //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                         //     -4.6V
      0x84,                         //     AUTO mode
    ST7735_PWCTR2,  1,              //  8: Power control, 1 arg, no delay:
      0xC5,                         //     VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
    ST7735_PWCTR3,  2,              //  9: Power control, 2 args, no delay:
      0x0A,                         //     Opamp current small
      0x00,                         //     Boost frequency
    ST7735_PWCTR4,  2,              // 10: Power control, 2 args, no delay:
      0x8A,                         //     BCLK/2,
      0x2A,                         //     opamp current small & medium low
    ST7735_PWCTR5,  2,              // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1,  1,              // 12: Power control, 1 arg, no delay:
      0x0E,
    ST77XX_INVOFF,  0,              // 13: Don't invert display, no args
    ST77XX_MADCTL,  1,              // 14: Mem access ctl (directions), 1 arg:
      0xC8,                         //     row/col addr, bottom-top refresh
    ST77XX_COLMOD,  1,              // 15: set color mode, 1 arg, no delay:
      0x05 },                       //     16-bit color

  Rcmd2green[] = {                  // 7735R init, part 2 (green tab only)
    2,                              //  2 commands in list:
    ST77XX_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,                   //     XSTART = 0
      0x00, 0x7F+0x02,              //     XEND = 127
    ST77XX_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,                   //     XSTART = 0
      0x00, 0x9F+0x01 },            //     XEND = 159

  Rcmd2red[] = {                    // 7735R init, part 2 (red tab only)
    2,                              //  2 commands in list:
    ST77XX_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x7F,                   //     XEND = 127
    ST77XX_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x9F },                 //     XEND = 159

  Rcmd2green144[] = {               // 7735R init, part 2 (green 1.44 tab)
    2,                              //  2 commands in list:
    ST77XX_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x7F,                   //     XEND = 127
    ST77XX_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x7F },                 //     XEND = 127

  Rcmd2green160x80[] = {            // 7735R init, part 2 (mini 160x80)
    2,                              //  2 commands in list:
    ST77XX_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x4F,                   //     XEND = 79
    ST77XX_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x9F },                 //     XEND = 159

  Rcmd3[] = {                       // 7735R init, part 3 (red or green tab)
    4,                              //  4 commands in list:
    ST7735_GMCTRP1, 16      ,       //  1: Gamma Adjustments (pos. polarity), 16 args + delay:
      0x02, 0x1c, 0x07, 0x12,       //     (Not entirely necessary, but provides
      0x37, 0x32, 0x29, 0x2d,       //      accurate colors)
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      ,       //  2: Gamma Adjustments (neg. polarity), 16 args + delay:
      0x03, 0x1d, 0x07, 0x06,       //     (Not entirely necessary, but provides
      0x2E, 0x2C, 0x29, 0x2D,       //      accurate colors)
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST77XX_NORON,     ST_CMD_DELAY, //  3: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON,    ST_CMD_DELAY, //  4: Main screen turn on, no args w/delay
      100                           //     100 ms delay
    };  

    static GPIO_TypeDef* LCD_GPIO = GPIOC;
    static GPIO_TypeDef* SPI2_GPIO = GPIOB;

    void sendCommand(SPI_HandleTypeDef* hspi, uint8_t cmd, const uint8_t* addr, uint8_t num)
    {
        HAL_GPIO_WritePin(LCD_GPIO, LCD_DC, GPIO_PIN_RESET);
        HAL_SPI_Transmit(hspi, &cmd, 1, 100);
        HAL_GPIO_WritePin(LCD_GPIO, LCD_DC, GPIO_PIN_SET);
        HAL_SPI_Transmit(hspi, const_cast<uint8_t*>(addr), num, 100);
    }

    void writeCommand(SPI_HandleTypeDef* hspi, uint8_t cmd)
    {
      HAL_GPIO_WritePin(LCD_GPIO, LCD_DC, GPIO_PIN_RESET);
      HAL_SPI_Transmit(hspi, &cmd, 1, 100);
      HAL_GPIO_WritePin(LCD_GPIO, LCD_DC, GPIO_PIN_SET);
    }

    // adafruit st7735 lib.
    //https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST77xx.cpp
    void shovelCommandsOffSomehow(SPI_HandleTypeDef* hspi, const uint8_t *addr)
    {
        uint8_t  numCommands, cmd, numArgs;
        uint16_t ms;
        numCommands = *addr++;
        while(numCommands--)
        {
            cmd = *addr++;
            numArgs  = *addr++;
            ms       = numArgs & ST_CMD_DELAY;
            numArgs &= ~ST_CMD_DELAY;
            sendCommand(hspi, cmd, addr, numArgs);
            addr += numArgs;

            if(ms)
            {
                ms = *addr++;
                if(ms == 255) ms = 500;
                HAL_Delay(ms);
            }
        }
    }
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  while(true);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  while(true);
}

DisplayST7735::DisplayST7735()
{
}

DisplayST7735::~DisplayST7735()
{
    HAL_SPI_DeInit(&hspi_);
}

void DisplayST7735::init()
{
    // GPIO:
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = LCD_CS | LCD_RST | LCD_DC;
    HAL_GPIO_Init(LCD_GPIO, &GPIO_InitStruct);

    // Init alternate function on SPI2 pins.
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = SPI2_MOSI | SPI2_SCLK | SPI2_NSS;
    HAL_GPIO_Init(SPI2_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pin = SPI2_MISO;
    HAL_GPIO_Init(SPI2_GPIO, &GPIO_InitStruct);

    // DMA init.
    DMA_InitTypeDef DMAInitDef = {};
    DMAInitDef.Direction = DMA_MEMORY_TO_PERIPH;
    DMAInitDef.PeriphInc = DMA_PINC_ENABLE;
    DMAInitDef.MemInc = DMA_MINC_ENABLE;
    DMAInitDef.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMAInitDef.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    DMAInitDef.Mode = DMA_NORMAL;
    DMAInitDef.Priority = DMA_PRIORITY_HIGH;

    hdma_ = {};
    hdma_.Instance = DMA1_Channel5;
    hdma_.Init = DMAInitDef;
    HAL_DMA_Init(&hdma_);
    
    // SPI2 init.
    SPI_InitTypeDef SPIInitDef = {};
    SPIInitDef.Mode = SPI_MODE_MASTER;
    SPIInitDef.Direction = SPI_DIRECTION_1LINE;
    SPIInitDef.DataSize = SPI_DATASIZE_8BIT;
    SPIInitDef.CLKPolarity = SPI_POLARITY_LOW;
    SPIInitDef.CLKPhase = SPI_PHASE_1EDGE;
    SPIInitDef.NSS = SPI_NSS_SOFT;
    SPIInitDef.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    SPIInitDef.FirstBit = SPI_FIRSTBIT_MSB;
    SPIInitDef.TIMode = SPI_TIMODE_DISABLE;
    SPIInitDef.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPIInitDef.CRCPolynomial = 0;

    hspi_ = {};
    hspi_.Instance = SPI2;
    hspi_.Init = SPIInitDef;
    hspi_.hdmatx = &hdma_;

    __HAL_RCC_SPI2_CLK_ENABLE();

    HAL_SPI_Init(&hspi_);

    // Do reset.
    HAL_GPIO_WritePin(LCD_GPIO, LCD_RST, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_GPIO, LCD_RST, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO, LCD_RST, GPIO_PIN_SET);

    begin();

    // TODO: make betterer.
    shovelCommandsOffSomehow(&hspi_, Rcmd1);

    colstart_ = 1;
    rowstart_ = 2;

    HAL_GPIO_WritePin(LCD_GPIO, LCD_CS, GPIO_PIN_RESET);
    uint8_t madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    sendCommand(&hspi_, ST77XX_MADCTL, &madctl, 1);

    //
    setAddrWindow(0, 0, 128, 128);

    uint16_t black[128];
    for(int i = 0; i < 128; ++i)
      black[i] = 0;

    for(int i = 0; i < 128; ++i)
    {
      HAL_SPI_Transmit(&hspi_, (uint8_t*)&black[0], 128 * 2, 100);
      //HAL_SPI_Transmit_DMA(&hspi_, (uint8_t*)&black[0], 128 * 2);

    }

    // Turn on display.
    shovelCommandsOffSomehow(&hspi_, Rcmd3);

    end();
}

void DisplayST7735::setAddrWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    x += colstart_;
    y += rowstart_;
    uint32_t xa = ((uint32_t)x << 16) | (x+w-1);
    uint32_t ya = ((uint32_t)y << 16) | (y+h-1);
    uint8_t* pxa = (uint8_t*)&xa;
    uint8_t* pya = (uint8_t*)&ya;

    writeCommand(&hspi_, ST77XX_CASET);
    HAL_SPI_Transmit(&hspi_, &pxa[3], 1, 100);
    HAL_SPI_Transmit(&hspi_, &pxa[2], 1, 100);
    HAL_SPI_Transmit(&hspi_, &pxa[1], 1, 100);
    HAL_SPI_Transmit(&hspi_, &pxa[0], 1, 100);

    writeCommand(&hspi_, ST77XX_RASET);
    HAL_SPI_Transmit(&hspi_, &pya[3], 1, 100);
    HAL_SPI_Transmit(&hspi_, &pya[2], 1, 100);
    HAL_SPI_Transmit(&hspi_, &pya[1], 1, 100);
    HAL_SPI_Transmit(&hspi_, &pya[0], 1, 100);

    writeCommand(&hspi_, ST77XX_RAMWR); // write to RAM
}
#endif

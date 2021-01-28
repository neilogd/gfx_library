#include "canvas.h"
#include "tile_canvas.h"
#include "profiling.h"

#define PROGMEM
#include "fonts/Picopixel.h"
#include "fonts/Org_01.h"

extern const char* ExampleName;
extern void ExampleInit();
extern void ExampleInput(uint16_t id, uint16_t val);
extern void ExampleTick(Canvas& canvas);


#if defined(PLATFORM_PC)
#include <SDL2/SDL.h>

#include <sys/time.h>
#include <vector>

class SDL2_Canvas : public Canvas
{
public:
    SDL2_Canvas(SDL_Renderer* renderer)
        : Canvas(128,128)
    {
        texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, 128, 128);
    }

    ~SDL2_Canvas()
    {
        SDL_DestroyTexture(texture_);
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) override
    {
        for(int j = 0; j < h; ++j)
        {
            for(int i = 0; i < w; ++i)
            {
                SDL_Rect rect = { x + i, y + j, 1, 1 };
                SDL_UpdateTexture(texture_, &rect, data++, 2);
            }
        }
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) override
    {
        for(int j = 0; j < h; ++j)
        {
            for(int i = 0; i < w; ++i)
            {
                SDL_Rect rect = { x + i, y + j, 1, 1 };
                SDL_UpdateTexture(texture_, &rect, &c, 2);
            }
        }
    }

    SDL_Texture* texture_;
};

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    char windowTitle[256];
    sprintf(windowTitle, "GFX Library Example: %s", ExampleName);

    SDL_Window* window = SDL_CreateWindow(
        windowTitle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        512, 512,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, 0, SDL_RENDERER_ACCELERATED
    );

    SDL2_Canvas canvas(renderer);

    ExampleInit();

    SDL_Event ev;
    for(;;)
    {
        canvas.setFont(&Picopixel);

        if(SDL_PollEvent(&ev))
        {
            switch(ev.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_KEYDOWN:
                ExampleInput(ev.key.keysym.sym, true);
                break;
            case SDL_KEYUP:
                ExampleInput(ev.key.keysym.sym, false);
            default:
                break;
            }
        }

        ExampleTick(canvas);

        SDL_Rect srcRect = { 0, 0, 128, 128 };
        SDL_Rect dstRect = { 0, 0, 512, 512 };

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, canvas.texture_, &srcRect, &dstRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#elif defined(PLATFORM_STM32)

#include "stm32f1xx_hal.h"
#include "isr.h"


template<>
struct IRQHandler<VectorTableEntry::Systick>
{
    static constexpr auto LambdaHandler = [](){ 
        HAL_IncTick();
        HAL_SYSTICK_IRQHandler();
    };
    using constant_t = std::integral_constant<void(*)(), +LambdaHandler>;
};


template<>
struct IRQHandler<VectorTableEntry::Dma1Channel5>
{
    static constexpr auto LambdaHandler = [](){ 
        while(true);
    };
    using constant_t = std::integral_constant<void(*)(), +LambdaHandler>;
};

#include "vectors.h"

#include "display_st7735.h"

SPI_HandleTypeDef s_hspi2;

class Dummy_Canvas final : public Canvas
{
public:
    DisplayST7735 display;

    Dummy_Canvas(const DisplayConfig& config)
        : Canvas(128,128)
    {
        display.init(config);
    }

    ~Dummy_Canvas()
    {
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) override
    {
        ProfilingTimestamp("writePixels BEGIN");
        display.setAddrWindow(x, y, w, h);
        display.writePixels(data, w * h);
        ProfilingTimestamp("writePixels END");
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) override
    {
        display.setAddrWindow(x, y, w, h);
        for(int j = 0; j < h; ++j)
        {
            for(int i = 0; i < w; ++i)
            {
                display.writePixels(&c, 1);
            }
        }
    }
};

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                    RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_ACR_LATENCY_2);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

static GPIO_TypeDef* LCD_GPIO = GPIOC;
static GPIO_TypeDef* SPI2_GPIO = GPIOB;

static const uint16_t LCD_CS = GPIO_PIN_13;
static const uint16_t LCD_RST = GPIO_PIN_14;
static const uint16_t LCD_DC = GPIO_PIN_15;

static const uint16_t SPI2_MOSI = GPIO_PIN_15;
static const uint16_t SPI2_MISO = GPIO_PIN_14;
static const uint16_t SPI2_SCLK = GPIO_PIN_13;
static const uint16_t SPI2_NSS = GPIO_PIN_12;

void GPIO_Config()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
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
}

void SPI_Config()
{
    __HAL_RCC_SPI2_CLK_ENABLE();

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

    s_hspi2 = {};
    s_hspi2.Instance = SPI2;
    s_hspi2.Init = SPIInitDef;

    HAL_SPI_Init(&s_hspi2);
}

int main()
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Config();
    SPI_Config();

    ExampleInit();

    DisplayConfig config = 
    {
        0, 0, // row/col start
        &s_hspi2, // hspi
        nullptr, // hdma
    };

    Dummy_Canvas canvas(config);

    uint32_t tick = 0;
    uint32_t timer = 0;
    char frameTime[16];

    while(true)
    {
        tick = DWT->CYCCNT;
        canvas.setFont(&Picopixel);

        canvas.display.begin();
        ExampleTick(canvas);
        canvas.setColors(0xffff, 0x0000);
        canvas.setFont(&Picopixel);
        canvas.drawText(96, 0, frameTime);
        canvas.display.end();
        timer = (DWT->CYCCNT - tick) / (SystemCoreClock / 1000000);
        //sprintf(frameTime, "%u us", timer);
    }

    return 0;
}

#ifdef USE_FULL_ASSERT
extern "C" void assert_failed(uint8_t* file, uint32_t line)
{
    char error[64]; 
    const char* lastSeparator = (const char*)file;
    while(*file != '\0')
    {
        if(*file == '\\' || *file == '/')
            lastSeparator = (const char*)file + 1;
        file++;
    }

    sprintf(error, "Assertion Failed:\n  %s:%i", lastSeparator, line);

    DisplayConfig config = 
    {
        0, 0, // row/col start
        &s_hspi2, // hspi
        nullptr, // hdma
    };

    Dummy_Canvas canvas(config);
    canvas.display.begin();
    canvas.setColors(COLOR_RED, COLOR_BLACK);
    canvas.drawFilledBox(0, 0, 128, 128);
    canvas.setColors(COLOR_WHITE, COLOR_RED);
    canvas.setFont(&Picopixel);
    canvas.drawText(0, 0, error);
    canvas.display.end();
    while(true);
}
#endif

#elif defined(PLATFORM_RPI_RP2)

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "display_st7735.h"

class Dummy_Canvas final : public Canvas
{
public:
    DisplayST7735 display;

    Dummy_Canvas(const DisplayConfig& config)
        : Canvas(128,128)
    {
        display.init(config);
    }

    ~Dummy_Canvas()
    {
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) override
    {
        ProfilingTimestamp("writePixels BEGIN");
        display.setAddrWindow(x, y, w, h);
        display.writePixels(data, w * h);
        ProfilingTimestamp("writePixels END");
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) override
    {
        display.setAddrWindow(x, y, w, h);
        for(int j = 0; j < h; ++j)
        {
            for(int i = 0; i < w; ++i)
            {
                display.writePixels(&c, 1);
            }
        }
    }
};

static const uint16_t LCD_CS = 5;
static const uint16_t LCD_RST = 6;
static const uint16_t LCD_DC = 7;

static const uint16_t SPI2_MOSI = 3;
static const uint16_t SPI2_MISO = 4;
static const uint16_t SPI2_SCLK = 2;
static const uint16_t SPI2_NSS = 5;

int main()
{
    stdio_init_all();
    spi_init(spi0, 24 * 1000 * 1000);
    gpio_set_function(SPI2_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI2_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI2_MOSI, GPIO_FUNC_SPI);

    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_init(LCD_CS);
    gpio_init(LCD_RST);
    gpio_init(LCD_DC);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_put(LCD_CS, 1);
    gpio_put(LCD_RST, 1);
    gpio_put(LCD_DC, 0);

    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);


    
    ExampleInit();

    DisplayConfig config = 
    {
        1, 2, // row/col start
    };

    Dummy_Canvas canvas(config);

    uint32_t tick = 0;
    uint32_t timer = 0;
    char frameTime[16];

    while(true)
    {
        tick = 0;
        canvas.setFont(&Picopixel);

        canvas.display.begin();
        ExampleTick(canvas);
        canvas.setColors(0xffff, 0x0000);
        canvas.setFont(&Picopixel);
        canvas.drawText(96, 0, frameTime);

        gpio_put(LED_PIN, 1);
        canvas.display.end();
        gpio_put(LED_PIN, 0);
        //timer = (0 - tick) / (1 / 1000000);
        //sprintf(frameTime, "%u us", timer);
    }

    return 0;
}

#ifdef USE_FULL_ASSERT
extern "C" void assert_failed(uint8_t* file, uint32_t line)
{
    char error[64]; 
    const char* lastSeparator = (const char*)file;
    while(*file != '\0')
    {
        if(*file == '\\' || *file == '/')
            lastSeparator = (const char*)file + 1;
        file++;
    }

    sprintf(error, "Assertion Failed:\n  %s:%i", lastSeparator, line);

    DisplayConfig config = 
    {
        0, 0, // row/col start
        &s_hspi2, // hspi
        nullptr, // hdma
    };

    Dummy_Canvas canvas(config);
    canvas.display.begin();
    canvas.setColors(COLOR_RED, COLOR_BLACK);
    canvas.drawFilledBox(0, 0, 128, 128);
    canvas.setColors(COLOR_WHITE, COLOR_RED);
    canvas.setFont(&Picopixel);
    canvas.drawText(0, 0, error);
    canvas.display.end();
    while(true);
}
#endif

#endif


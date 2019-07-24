#include "canvas.h"
#include "tile_canvas.h"

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
#else

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

#include "vectors.h"

class Dummy_Canvas : public Canvas
{
public:
    Dummy_Canvas()
        : Canvas(128,128)
    {
    }

    ~Dummy_Canvas()
    {
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) override
    {
        for(int j = 0; j < h; ++j)
        {
            for(int i = 0; i < w; ++i)
            {
                // do nothing
            }
        }
    }

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) override
    {
        for(int j = 0; j < h; ++j)
        {
            for(int i = 0; i < w; ++i)
            {
                // do nothing
            }
        }
    }
};

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void GPIO_Config()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    /* Set GPIOD Pin#15 Parameters */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int main()
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Config();

    ExampleInit();

    Dummy_Canvas canvas;

    while(true)
    {
        ExampleTick(canvas);

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(33); // ~30fps tick, ish.
    }

    return 0;
}
#endif


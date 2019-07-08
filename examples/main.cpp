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

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data) override
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

#include "isr.h"

uint32_t Systick_Count = 0;
uint32_t RTC_Count = 0;

template<>
struct IRQHandler<VectorTableEntry::Systick>
{
    static constexpr auto LambdaHandler = [](){ Systick_Count++; };
    using constant_t = std::integral_constant<void(*)(), +LambdaHandler>;
};

template<>
struct IRQHandler<VectorTableEntry::RTC>
{
    static constexpr auto LambdaHandler = [](){ RTC_Count++; };
    using constant_t = std::integral_constant<void(*)(), +LambdaHandler>;
};

#include "vectors.h"

#include "stm32f1xx_hal.h"

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

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data) override
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


int main()
{
    HAL_Init();

    ExampleInit();

    Dummy_Canvas canvas;

    while(true)
    {
        ExampleTick(canvas);
    }

    return 0;

}
#endif


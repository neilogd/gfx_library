#include "canvas.h"
#include "tile_canvas.h"

#define PROGMEM
#include "fonts/Picopixel.h"
#include "fonts/Org_01.h"

#define PLATFORM_PC

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

inline double getSeconds()
{
	timeval time;
	::gettimeofday( &time, NULL );
	return time.tv_sec + ( (double)time.tv_usec / 1000000.0 );
}

inline double getMicroseconds()
{
    return getSeconds() * 1000000.0;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "LCD Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        512, 512,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, 0, SDL_RENDERER_ACCELERATED
    );

    SDL2_Canvas canvas(renderer);

    uint8_t cmdListBuffer[1024];
    CommandList cmdList(cmdListBuffer, sizeof(cmdListBuffer));

    const int16_t TILE_WIDTH = 16;
    const int16_t TILE_HEIGHT = 16;
    uint16_t tileBuffer[TILE_WIDTH * TILE_HEIGHT];
    std::vector<TileCanvas> tileCanvases;
    for(int j = 0; j < 128 / TILE_HEIGHT; ++j)
    {
        for(int i = 0; i < 128 / TILE_WIDTH; ++i)
        {
            tileCanvases.emplace_back(tileBuffer,  i * TILE_WIDTH,  j * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
        }
    }

    static bool debugShowTiles = false;
    static bool debugHashTiles = false;

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
                if(!debugShowTiles && ev.key.keysym.sym == 't')
                    debugShowTiles = true;
                if(!debugHashTiles && ev.key.keysym.sym == 'h')
                    debugHashTiles = true;
                break;
            case SDL_KEYUP:
                if(debugShowTiles && ev.key.keysym.sym == 't')
                    debugShowTiles = false;
                if(debugHashTiles && ev.key.keysym.sym == 'h')
                    debugHashTiles = false;
                break;
            default:
                break;
            }
        }

        cmdList.clear();

        cmdList.drawFilledBox(0, 0, 128, 128, Color565From888(0, 0, 64));

        const char* testString = "ABCDEFGHIJK\nLMNOPQRSTUV\nWXYZ\n1234567890\n!@#$%^&*()\n[]<>;:'-+=\n\n";
        int16_t texty = 0;
        cmdList.drawText(8, texty, testString, &Org_01, 0xffff, 0x0000);
        texty += 48;

        cmdList.drawFilledBox(0, texty, 64, texty + 16, Color565From888(255, 0, 255));
        cmdList.drawBox(0, texty, 64, texty + 16, Color565From888(255, 255, 255));
        cmdList.drawText(8, texty, testString, &Picopixel, 0xffff, 0xffff);
        texty += 48;

        static int16_t bx = 92;
        static int16_t by = 98;
        static int16_t dx = 2;
        static int16_t dy = 1;

        bx += dx;
        by += dy;
        if(bx >= 128 || bx <= 0) dx = -dx;
        if(by >= 128 || by <= 0) dy = -dy;
        cmdList.drawBox(bx-1, by-1, 3, 3, Color565From888(255, 0, 0));
        
#if 1
        static double debugTileExecuteTime = 0.0;
        static double debugTilePresentTime = 0.0;
        static double debugNumTileSamples = 0.0;

        static double debugFrameTime = 0.0;
        static double debugNumFrameSamples = 0.0;

        debugFrameTime -= getMicroseconds();
        for(auto& tileCanvas : tileCanvases)
        {
            debugTileExecuteTime -= getMicroseconds();
            tileCanvas.executeCommandList(cmdList);
            debugTileExecuteTime += getMicroseconds();

            debugTilePresentTime -= getMicroseconds();
            bool presentedTile = tileCanvas.present(canvas, debugHashTiles);
            debugTilePresentTime += getMicroseconds();

            debugNumTileSamples += 1.0;

            if(debugShowTiles)
            {
                debugFrameTime += getMicroseconds();
                uint16_t debugColor = Color565From888(255, 0, 0);
                if(presentedTile)
                    debugColor = Color565From888(0, 255, 0);
                canvas.drawBox(tileCanvas.x_, tileCanvas.y_, tileCanvas.w_, tileCanvas.h_, debugColor);
                debugFrameTime -= getMicroseconds();
            }
        }
        debugFrameTime += getMicroseconds();
        debugNumFrameSamples += 1.0;

#else
        canvas.executeCommandList(cmdList);
#endif

        static int logCount = 0;
        if(logCount++ > 30)
        {
            const int32_t cmdBufferSize = cmdList.end() - cmdList.begin(); 
            printf("Stats:\n");
            printf("- CommandList size: %d bytes\n", cmdBufferSize);
            printf("- Tiles: %d\n", (int)tileCanvases.size());
            printf("- Avg. execute per tile: %f us\n", debugTileExecuteTime / debugNumTileSamples);
            printf("- Avg. present per tile: %f us\n", debugTilePresentTime / debugNumTileSamples);
            printf("- Avg. frame time: %f us\n", debugFrameTime / debugNumFrameSamples);
            debugTileExecuteTime = 0.0;
            debugTilePresentTime = 0.0;
            debugNumTileSamples = 0.0;
            debugFrameTime = 0.0;
            debugNumFrameSamples = 0.0;
            logCount = 0;
        }

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
int main()
{


    return 0;
}
#endif


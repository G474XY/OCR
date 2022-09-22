#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Color[][] GetPixelColor(const SDL_Surface* pSurface)
{
    SDL_Color Tab_Color[1000][1000];
    for (int X = 0; X < 1000; X++)
    {
        for (int Y = 0; Y < 1000; Y++)
        {
            const Uint8 Bpp = pSurface->format->BytesPerPixel;
            Uint8* pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;

            Uint32 PixelData = *(Uint32*)pPixel;

            SDL_Color Color = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};

            SDL_GetRGB(PixelData, pSurface->format, &Color.r, &Color.g, &Color.b);

            Tab_Color[X][Y] = Color;
        }
    }
    return Tab_Color;
}

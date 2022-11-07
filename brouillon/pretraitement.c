#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint32 color = SDL_MapRGB(format, r, g, b);

    Uint8 average = 0.3*r + 0.59*g + 0.11*b;

    r = average;
    g = average;
    b = average;
    
    color = SDL_MapRGB(format, r, g, b);
    return color;
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++)
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    SDL_UnlockSurface(surface);
}

//moyenne d'un pixel avec ces voisins pour le flou gaussien
Uint32 moyenne(SDL_Surface *surface, int i, int j, int n)
{
    const int initial_h = SDL_max(i - n, 0);
    const int initial_w = SDL_max(j - n, 0);
    const int final_h = SDL_min(i + n, surface->h - 1);
    const int final_w = SDL_min(j + n, surface->w - 1);
    const int nb_pixel = ((final_h - initial_h) * (final_w - initial_w));
    const Uint32 *p = surface->pixels;

    Uint32 sum_r = 0, sum_g = 0, sum_b = 0;
    SDL_Color color;

    for (i = initial_h; i < final_h; i++)
        for(j = initial_w; j < final_w; j++)
        {
            SDL_GetRGB(p[i * surface->w + j], surface->format, &color.r, &color.g, &color.b);
            sum_r += color.r;
            sum_g += color.g;
            sum_b += color.b;
        }

    return SDL_MapRGB(surface->format, sum_r / nb_pixel, sum_g / nb_pixel, sum_b / nb_pixel);
}

void flou_gaussien(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int h = surface->h;
    int w = surface->w;
    for(int i = 0; i < h; i++)
    {
    for(int j = 0; j < w; j++)
        {
            pixels[i * w + j] = moyenne(surface, i, j, 2);
        }
    }
}

//methode d'otsu pour trouver un threshold
double find_threshold(SDL_Surface* surface, int h1, int h2, int w1, int w2)
{
    Uint32* pixels = surface->pixels;
    int w = surface->w;
    int histo[256];
    for (int i = 0; i < 256;i++)
    {
        histo[i] = 0;
    }

    for (int i = h1; i < h2; i++)
    {
        for (int j = w1; j < w2; j++)
        {
            Uint8 r, g, b;
            SDL_PixelFormat* format = surface->format;
            SDL_GetRGB(pixels[i*w+j], format, &r, &g, &b);
            histo[r] += 1;
        }
    }
    int sum = 0;
    for (int k = 0; k < 256; k ++)
    {
        sum += k * histo[k];
    }

    int total = (h2 - h1) * (w2 - w1);

    double sumB = 0;
    int wB = 0;
    int wF = 0;

    double varMax = 0;
    double threshold = 0;

    for (int t=0 ; t<256 ; t++) {
        wB += histo[t];
        if (wB == 0) continue;

        wF = total - wB;
        if (wF == 0) break;

        sumB += (double) (t * histo[t]);

        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double varBetween = (double)wB * (double)wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
        varMax = varBetween;
        threshold = t;
        }
    }
    return threshold;
}

//n est le nombre de cases que l'on veut pour avoir differents thresholds
void binarization(SDL_Surface* surface, int n)
{
    SDL_PixelFormat* format = surface->format;
    int h = surface->h;
    int w = surface->w;
    int h1 = 0;
    int h2 = 0;
    int w1 = 0;
    int w2 = 0;
    SDL_LockSurface(surface);
    for (int x = 0; x < n; x++)
    {
        printf("la");
        for (int y = 0; y < n; y++)
        {
            h1 = x*h/n;
            h2 = (x+1)*h/n;
            w1 = y*w/n;
            w2 = (y+1)*w/n;
            int t = (int) find_threshold(surface, h1, h2, w1, w2);
            printf("%d-%d-%d-%d-%d-%d-%d\n", t, h1, h2, w1, w2, x, y);
            Uint32* pixels = surface->pixels;
            for (int i = h1; i < h2; i++)
            {
                for (int j = w1; j < w2; j++)
                {
                    Uint8 r, g, b;
                    SDL_GetRGB(pixels[i*(w)+j], format, &r, &g, &b);
                    if (r > t*0.8)
                    {
                        r = 255;
                        g = 255;
                        b = 255;
                        pixels[i*(w)+j] = SDL_MapRGB(format, r, g, b);
                    }
                    else
                    {
                        r = 0;
                        g = 0;
                        b = 0;
                        pixels[i*(w)+j] = SDL_MapRGB(format, r, g, b);
                    }
                }
            }
        }
    }
    SDL_UnlockSurface(surface);
}

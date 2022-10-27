#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale)
{
    SDL_Event event;
    SDL_Texture* t = grayscale;
    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                return;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    draw(renderer, t);
                break;
            /*
            case SDL_KEYDOWN:
                draw(renderer, t);
                if (t == colored)
                    t = grayscale;
                else
                    t = colored;*/
            default:
                draw(renderer, t);
        }
    }
}

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* s1 = IMG_Load(path);
    if (s1 == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* s2 = SDL_ConvertSurfaceFormat(s1, SDL_PIXELFORMAT_RGB888, 0);
    if (s2 == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(s1);
    return s2;
}

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
static Uint32 moyenne(SDL_Surface *surface, int i, int j, int n)
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


float find_threshold(SDL_Surface* surface, int h1, int h2, int w1, int w2)
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

    float sumB = 0;
    int wB = 0;
    int wF = 0;

    float varMax = 0;
    float threshold = 0;

    for (int t=0 ; t<256 ; t++) {
        wB += histo[t];
        if (wB == 0) continue;

        wF = total - wB;
        if (wF == 0) break;

        sumB += (float) (t * histo[t]);

        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
        varMax = varBetween;
        threshold = t;
        }
    }
    return threshold;
}

void binarization(SDL_Surface* surface, int n)
{
    SDL_PixelFormat* format = surface->format;
    int h = surface->h;
    int w = surface->w;
    int h1 = 0;
    int h2 = 0;
    int w1 = 0;
    int w2 = 0;
    int x = 0;
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
                    if (r > t)
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


int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    //Initialize the SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a window.
    SDL_Window* window = SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0,
      640, 400,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
    SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a surface from the colored image.
    SDL_Surface* s = load_image(argv[1]);

    // - Resize the window according to the size of the image.
    SDL_SetWindowSize(window, s->w, s->h);
    // - Create a texture from the colored surface.
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, s);
    // - Convert the surface into grayscale.
    surface_to_grayscale(s);
    flou_gaussien(s);
    binarization(s, 2);
    // - Create a new texture from the grayscale surface.
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, s);
    // - Free the surface.
    SDL_FreeSurface(s);
    // - Dispatch the events.
    event_loop(renderer, texture1, texture2);
    // - Destroy the objects.
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

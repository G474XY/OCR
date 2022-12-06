#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "pretraitement.h"
#include "detect_grid.h"

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
void event_loop(SDL_Renderer* renderer, SDL_Texture* s1, SDL_Texture* s2, 
        SDL_Texture* s3, SDL_Texture* s4, SDL_Texture* s5, SDL_Texture* s6,
        SDL_Texture* s7, SDL_Texture* s8)
{
    SDL_Event event;
    SDL_Texture* t = s1;
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

            case SDL_KEYDOWN:
                draw(renderer, t);
                if (t == s1)
                    t = s2;
                else if (t == s2)
                    t = s3;
                else if (t == s3)
                    t = s4;
                else if (t == s4)
                    t = s5;
                else if (t == s5)
                    t = s6;
                else if (t == s6)
                    t = s7;
                else if (t == s7)
                    t = s8;
                else
                    t = s1;
                break;
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
    // - Treatement Image.

    surface_to_grayscale(s);
    SDL_Surface* s1 = copy_s(s);
    IMG_SavePNG(s1, "s1.png");
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, s);

    change_luminosity(s);
    SDL_Surface* s2 = copy_s(s);
    IMG_SavePNG(s2, "s2.png");
    SDL_Texture* texture3 = SDL_CreateTextureFromSurface(renderer, s);

    change_contrast(s);
    SDL_Surface* s3 = copy_s(s);
    IMG_SavePNG(s3, "s3.png");
    SDL_Texture* texture4 = SDL_CreateTextureFromSurface(renderer, s);

    //flou_gaussien(s);
    SDL_Texture* texture5 = SDL_CreateTextureFromSurface(renderer, s);

    binarization(s,1);
    SDL_Surface* s4 = copy_s(s);
    IMG_SavePNG(s4, "s4.png");
    SDL_Texture* texture6 = SDL_CreateTextureFromSurface(renderer, s);

    white_to_black(s);
    SDL_Surface* s5 = copy_s(s);
    IMG_SavePNG(s5, "s5.png");
    SDL_Texture* texture7 = SDL_CreateTextureFromSurface(renderer, s);

    sobel(s);
    SDL_Surface* s6 = copy_s(s);
    IMG_SavePNG(s6, "s6.png");
    SDL_Texture* texture8 = SDL_CreateTextureFromSurface(renderer, s);
    // - Free the surface.
    SDL_FreeSurface(s);
    // - Dispatch the events.
    event_loop(renderer, texture1, texture2, texture3, texture4, texture5, 
            texture6, texture7, texture8);
    // - Destroy the objects.
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyTexture(texture4);
    SDL_DestroyTexture(texture5);
    SDL_DestroyTexture(texture6);
    SDL_DestroyTexture(texture7);
    SDL_DestroyTexture(texture8);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}


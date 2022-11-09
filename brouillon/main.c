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
void event_loop(SDL_Renderer* renderer, SDL_Texture* grayscale)
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
    flou_gaussien(s);
    binarization(s,1);
    white_to_black(s);
    SDL_Surface* s1 = crop_surface(s);
    IMG_SavePNG(s1, "out.png");
    // - Create a new texture from the grayscale surface.
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, s);
    // - Free the surface.
    SDL_FreeSurface(s);
    // - Dispatch the events.
    event_loop(renderer, texture2);
    // - Destroy the objects.
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}


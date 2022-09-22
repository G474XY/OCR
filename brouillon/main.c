#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
int main(int argc, char *args[])
{
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window *window = SDL_CreateWindow("SDL2 Displaying Image",
                                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 1200, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface *image = IMG_Load("sudoku_02.jpeg");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    while (1)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            break;
        }

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_a:
                SDL_Surface *image1 = IMG_Load("output.png");
                SDL_Texture *texture1 = SDL_CreateTextureFromSurface(renderer, image1);
                SDL_FreeSurface(image1);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture1, NULL, NULL);
                SDL_RenderPresent(renderer);
                break;
            }
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();

    SDL_Color tab_color[][] = GetPixelColor(image);
    printf("%i", tab_color[234][543]);

    return 0;
    }

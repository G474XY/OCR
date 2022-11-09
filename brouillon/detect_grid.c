#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int is_valid(int x, int y, int * mask, SDL_Surface* s)
{
    int h = s->h;
    int w = s->w;
    if (x < h && y < w && x >= 0 && y >= 0 && mask[x*w+y] != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void propage(int n1, int n2, int* mask, SDL_Surface* s)
{
    int w = s->w;
    int h = s->h;
    for (int i = 0; i < h*w; i++)
    {
        if (mask[i] == n2)
            mask[i] = n1;

        if (mask[i] > n2)
            mask[i] -= 1;
    }
}

void check_neighbors(int x, int y, int *mask, SDL_Surface* s, int * swap)
{
    int w = s->w;
    if ((is_valid(x-1, y, mask,s) == 0) && (is_valid(x, y-1, mask,s) == 0))
    {
        mask[x*w+y] = 0;
    }
    else if ((is_valid(x-1, y, mask,s) == 1) && (is_valid(x, y-1, mask,s) == 1))
    {
        if (mask[(x-1)*w+y] >= mask[x*w+y-1])
        {
            mask[x*w+y] = mask[x*w+y-1];
            swap[mask[(x-1)*w+y]] = swap[mask[x*w+y-1]];
        }
        else
        {
            mask[x*w+y] = mask[(x-1)*w+y];
            swap[mask[x*w+y-1]] = swap[mask[(x-1)*w+y]];
        }
    }

    else if ((is_valid(x, y-1, mask,s) == 1))
    {
        mask[x*w+y] = mask[x*w+y-1];
        swap[mask[x*w+y-1]] = mask[x*w+y];
    }
    else
    {
        mask[x*w+y] = mask[(x-1)*w+y];
        swap[mask[(x-1)*w+y]] = mask[x*w+y];
    }
}

void resize_grid(SDL_Surface* surface, int values[])
{
    SDL_PixelFormat* format = surface->format;
    int h = surface->h;
    int w = surface->w;
    Uint32* pixels = surface->pixels;

    int* mask = calloc((h*w), sizeof(int));
    int *swap = calloc((h*w), sizeof(int));

    int index = 1;
    for (int i = 0; i < h;i++)
    {
        for(int j = 0; j < w;j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[i*(w)+j], format, &r, &g, &b);
            int actual = r;
            if (actual != 0)
            {
                check_neighbors(i, j, mask,surface, swap);
                if (mask[i*w+j] == 0)
                {
                    mask[i*w+j] = index;
                    index += 1;
                }
            }
            else
            {
                mask[i*w+j] = 0;
            }
        }
    }
    //met les valeurs de zones collees au plus petit chiffre
    for (int i = 0; i <= 1000; i ++)
    {
        if (swap[i] != 0)
            propage(swap[i], i, mask, surface);
    }
    //creer un histogramme avec le nombre de valeurs par zones
    int histo[index];
    for (int i = 0; i < index;i++)
    {
        histo[i] = 0;
    }

    for (int k = 0; k < h;k++)
    {
        for(int l = 0; l < w;l++)
        {
            int actual = mask[k*w+l];
            histo[actual] += 1;
        }
    }

    // trouve le nombre de la zone la plus grande
    int i = 1;
    int max = 0;
    int number = 0;
    while (i < index)
    {
        if (histo[i] > max)
        {
            max = histo[i];
            number = i;
        }
        i+=1;
    }
    printf("number = %d\n", number);

    //garde que la plus grande zone
    for (int i = 0; i < h;i++)
    {
        for(int j = 0; j < w;j++)
        {
            if (mask[i*w+j] != number)
                mask[i*w+j] = 0;
        }
    }

    //ne garde que la matrice delimitee a la zone
    int h1 = h;
    int w1 = w;
    int h2 = 0;
    int w2 = 0;
    for (int i = 0; i < h;i++)
    {
        for(int j = 0; j < w;j++)
        {
            if (mask[i*w+j] == number)
            {
                if (i < h1)
                    h1 = i;
                if (i > h2)
                    h2 = i;
            }
        }
    }

    for (int k = 0; k < h;k++)
    {
        for(int l = 0; l < w;l++)
        {
            if (mask[k*w+l] == 1)
            {
                if (l < w1)
                    w1 = l;
                if (l > w2)
                    w2 = l;
            }
        }
    }
    values[0] = h1;
    values[1] = w1;
    values[2] = h2;
    values[3] = w2;
    free(mask);
    free(swap);
    printf("h : %d, w : %d \n", h, w);
}


SDL_Surface* crop_surface(SDL_Surface* sprite_sheet)
{
    int values[4];
    resize_grid(sprite_sheet, values);
    for (int i = 0; i < 4; i ++)
    {
        printf("%d\n", values[i]);
    }
    int x = values[0];
    int y = values[1];
    int height = values[2] - values[0];
    int width = values[3] - values[1];
    SDL_Surface* surface = SDL_CreateRGBSurface(sprite_sheet->flags, width, height, sprite_sheet->format->BitsPerPixel, sprite_sheet->format->Rmask, sprite_sheet->format->Gmask, sprite_sheet->format->Bmask, sprite_sheet->format->Amask);
    SDL_Rect rect = {x, y, width, height};
    SDL_BlitSurface(sprite_sheet, &rect, surface, 0);
    return surface;
}

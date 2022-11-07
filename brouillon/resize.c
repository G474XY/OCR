#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


SDL_Surface* crop_surface(SDL_Surface* sprite_sheet)
{
    int h = sprite_sheet->h;
    int w = sprite_sheet->w;
    int visited[h*w];
    int result[h*w];
    int values[4];
    grid_coordinates(sprite_sheet, values, visited, result);
    int x = values[0];
    int y = values[1];
    int width = values[2] - values[0];
    int height = values[3] - values[1];
    SDL_Surface* surface = SDL_CreateRGBSurface(sprite_sheet->flags, width, height, sprite_sheet->format->BitsPerPixel, sprite_sheet->format->Rmask, sprite_sheet->format->Gmask, sprite_sheet->format->Bmask, sprite_sheet->format->Amask);
    SDL_Rect rect = {x, y, width, height};
    SDL_BlitSurface(sprite_sheet, &rect, surface, 0);
    return surface;
}

int COUNT = 0;

// Function checks if a cell is valid
int is_valid(int x, int y, int key, SDL_Surface* surface, int visited[])
{
  SDL_PixelFormat* format = surface->format;
  int h = surface->h;
  int w = surface->w;
  Uint8 r, g, b;
  Uint32* pixels = surface->pixels;
  SDL_GetRGB(pixels[x*(w)+y], format, &r, &g, &b);
	if (x < h && y < w && x >= 0 && y >= 0)
	{
		if (visited[x*w+y] == 0 && r == key)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

// BFS to find all cells in
// connection with key = input[i][j]
void BFS (int x, int y, int i, int j, SDL_Surface* surface, int visited[])
{
  	SDL_PixelFormat* format = surface->format;
  	int h = surface->h;
  	int w = surface->w;
 	Uint8 r, g, b;
  	Uint32* pixels = surface->pixels;
  	SDL_GetRGB(pixels[x*(w)+y], format, &r, &g, &b);
	// terminating case for BFS
	if (x != y)
	{
		return;
	}

	visited[i * w +j] = 1;
	COUNT++;
  
	int x_move[] = {0, 0, 1, -1};
	int y_move[] = {1, -1, 0, 0};

	// checks all four points
	// connected with input[i][j]
	for (int k = 0; k < 4; k++)
	{
		if ((is_valid(i + y_move[k], j + x_move[k], pixels[i*w+j], surface)) == 1)
		{
			BFS(x, y, i + y_move[k], j + x_move[k], surface);
		}
	}
}


void reset_visited(SDL_Surface* surface, int visited[])
{
  	int h = surface->h;
  	int w = surface->w;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			visited[i*w+j] = 0;
		}
	}
}

// If a larger connected component is
// found this function is called to
// store information about that component.
void reset_result(int key, SDL_Surface* surface, int visited[], int result[])
{
  SDL_PixelFormat* format = surface->format;
  int h = surface->h;
  int w = surface->w;
  Uint32* pixels = surface->pixels;

  for (int i = 0; i < h; i++)
  {
	for (int j = 0; j < w; j++)
	{
      		Uint8 r, g, b;
      		SDL_GetRGB(pixels[i*(w)+j], format, &r, &g, &b);
		if (visited[i*w+j] == 1 && r == key)
		{
			result[i*w+j] = visited[i*w+j];
		}
		else
		{
			result[i*w+j] = 0;
		}
	}
    }
}

// function to calculate the
// largest connected component
// and return the coodinates of the component
void grid_coordinates(SDL_Surface* surface, int[] values, int visited[], int result[])
{
  int current_max = -1;
  
  SDL_PixelFormat* format = surface->format;
  int h = surface->h;
  int w = surface->w;
  Uint32* pixels = surface->pixels;

  for (int i = 0; i < h; i++)
  {
	for (int j = 0; j < w; j++)
	{
      		Uint8 r, g, b;
      		SDL_GetRGB(pixels[i*(w)+j], format, &r, &g, &b);
      
      		Uint8 r1, g1, b1;
      		SDL_GetRGB(pixels[i*(w)+j+1], format, &r1, &g1, &b1);
      
      		Uint8 r2, g2, b2;
      		SDL_GetRGB(pixels[(i+1)*(w)+j], format, &r2, &g2, &b2);
      
		reset_visited(surface, visited);
		COUNT = 0;

			// checking cell to the right
		if (j + 1 < w)
		{
			BFS(r, r1, i, j, surface, visited);
		}

			// updating result
		if (COUNT >= current_max)
		{
			current_max = COUNT;
			reset_result(r ,surface, visited, result);
		}
		reset_visited(surface, visited);
		COUNT = 0;

			// checking cell downwards
		if (i + 1 < h)
		{
			BFS(r, r2, i, j, surface, visited);
		}

			// updating result
		if (COUNT >= current_max)
		{
			current_max = COUNT;
			reset_result(r, surface, visited, result);
		}
	}
   }
  
    int w1 = 0;
    int h1 = 0;
    int h2 = 0;
    int w2 = 0;
    int b1 = 1;
    int b2 = 1;
    for (int i = 0; i < h; i++)
    {
    	for (int j = 0; j < w; j++)
	{
          if (result[i*w+j] == 1 && b1 == 1)
          {
             h1 = i;
             b1 = 0;
          }
          if (result[i*w+j] == 1)
          {
            h2 = i;
          }
        }
    }
  
    for (int i = 0; i < w; i++)
    {
	for (int j = 0; j < h; j++)
	{
          if (result[i*w+j] == 1 && b2 == 1)
          {
              	w1 = i;
              	b2 = 0;
          }
          if (result[i*w+j] == 1)
          {
              	w2 = i;
          }
        }
    }
    values[0] = h1;
    values[1] = w1;
    values[2] = h2;
    values[3] = w2;
}

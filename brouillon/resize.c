#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


SDL_Surface* crop_surface(SDL_Surface* sprite_sheet)
{
    int values[4];
    grid_coordinates(sprite_sheet, values);
    int x = values[0];
    int y = values[1];
    int width = values[2] - values[0];
    int height = values[3] - values[1];
    SDL_Surface* surface = SDL_CreateRGBSurface(sprite_sheet->flags, width, height, sprite_sheet->format->BitsPerPixel, sprite_sheet->format->Rmask, sprite_sheet->format->Gmask, sprite_sheet->format->Bmask, sprite_sheet->format->Amask);
    SDL_Rect rect = {x, y, width, height};
    SDL_BlitSurface(sprite_sheet, &rect, surface, 0);
    return surface;
}

int h = surface->h;
int w = surface->w;

int visited[h*w];
int result[h*w];

int COUNT = 0;

// Function checks if a cell is valid
bool is_valid(int x, int y, SDL_Surface* surface)
{
  SDL_PixelFormat* format = surface->format;
  int h = surface->h;
  int w = surface->w;
  Uint8 r, g, b;
  Uint32* pixels = surface->pixels;
  
  SDL_GetRGB(pixels[x*(w)+y], format, &r, &g, &b);
	if (x < h && y < w && x >= 0 && y >= 0)
	{
		if (visited[x][y] == 0 && r == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// BFS to find all cells in
// connection with key = input[i][j]
void BFS (int x, int y, int i, int j, SDL_Surface* surface)
{
  int h = surface->h;
  int w = surface->w;
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
		if ((is_valid(i + y_move[k], j + x_move[k], x, surface)) == true)
		{
			BFS(x, y, i + y_move[k], j + x_move[k], surface);
		}
	}
}


void reset_visited(SDL_Surface* surface)
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
void reset_result(SDL_Surface* surface)
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
			if (visited[i*w+j] == 1 && r == 0)
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
void grid_coordinates(SDL_Surface* surface, int[] values)
{
	int current_max = 0;
  
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
      
			reset_visited(surface);
			COUNT = 0;

			// checking cell to the right
			if (j + 1 < w)
			{
				BFS(r, r1, i, j, surface);
			}

			// updating result
			if (COUNT >= current_max)
			{
				current_max = COUNT;
				reset_result(surface);
			}
			reset_visited(surface);
			COUNT = 0;

			// checking cell downwards
			if (i + 1 < h)
			{
				BFS(r, r2, i, j, surface);
			}

			// updating result
			if (COUNT >= current_max)
			{
				current_max = COUNT;
				reset_result(surface);
			}
		}
  
  	int w1 = 0;
  	int h1 = 0;
    int h2 = 0;
  	int w2 = 0;
  	bool b1 = true;
    bool b2 = true;
  	for (int i = 0; i < n; i++)
	  {
		  for (int j = 0; j < m; j++)
		  {
          if (result[i*w+j] == 1 && b1)
          {
             h1 = i;
             b1 = false;
          }
          if (result[i*w+j] == 1)
          {
            h2 = i;
          }
        }
    }
  
  	for (int i = 0; i < m; i++)
	  {
		for (int j = 0; j < n; j++)
		{
          	if (result[i*w+j] == 1 && b2)
            {
              	w1 = i;
              	b2 = false;
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

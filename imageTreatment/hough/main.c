#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "rotate.h"
#include "hough.h"


// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture, struct coordonate *m, int size)
{
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	for (int i =0; i<size; i++)
	{
		SDL_RenderDrawLine(renderer, m[i].x1, m[i].y1, m[i].x2, m[i].y2);
	}
	//SDL_RenderDrawLine(renderer, 500, 0, 500, 2000);
	SDL_RenderPresent(renderer);
}
// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* s1, struct coordonate *m, int size)
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
					draw(renderer, t, m, size);
				break;
			default:
				draw(renderer, t, m, size);
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
	SDL_Window* window = SDL_CreateWindow("pretreatment", 0, 0,
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

	//Rotateimg(s, 45);


	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, s);
	SDL_SetWindowSize(window, s->w, s->h);
	int i =0;
	int rho =0;
	int theta = 0;
	int *m3;
	double *m = hough(s, &i, &rho, &theta, &m3);
	//printf("%d\n", m3[1000]);

	//struct poss *poss = analysise(m3, rho, theta, 2000);

	//double **positions = NULL;
	int *result = NULL;

	//double *positions = analyse(m3, 1200, &i, result, rho, theta);

	//printf("%f\n", positions[0]);

	struct coordonate *m2 = coor(m, &i, s);

	//printf("%d\n", i);

	SDL_FreeSurface(s);
	// - Dispatch the events.


	event_loop(renderer, texture, m2, i);


	// - Destroy the objects.


	//SDL_Texture *text = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, i, j);


	free(m);
	free(m2);
	free(m3);
	SDL_DestroyWindow(window);
	SDL_Quit();


	return EXIT_SUCCESS;
}
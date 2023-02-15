/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2023 erysdren (it/she)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 ******************************************************************************/

/*
 * headers
 */

/* std */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* sdl */
#include <SDL2/SDL.h>

/* qcvm */
#include "qcvm.h"

/* qclib */
#include "qclib.h"

/* constants */
#define WIDTH 640
#define HEIGHT 480
#define TITLE "QCPONG"
#define PROGS "../examples/qcpong/qcpong.dat"

/* macros */
#define RGBA(r, g, b, a) (unsigned int)((r << 24) | (g << 16) | (b << 8) | a)
#define ARGB(r, g, b, a) (unsigned int)((a << 24) | (r << 16) | (g << 8) | b)
#define PIXEL(x, y) ((unsigned int *)pixels)[(unsigned int)(x) * WIDTH + (unsigned int)(y)]

/* globals */
void *pixels;
SDL_Window *window;
SDL_Texture *texture;
SDL_Renderer *renderer;

/* draw pixel on screen buffer */
void export_drawpixel(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3 pos, col;
	unsigned char r, g, b;

	/* get parms */
	pos = qcvm_get_parm_vector(qcvm, 0);
	col = qcvm_get_parm_vector(qcvm, 1);

	if (pos.x > WIDTH - 1 || pos.y > HEIGHT - 1) return;
	if (pos.x < 0 || pos.y < 0) return;

	/* convert color values */
	r = (unsigned char)(col.x * 255);
	g = (unsigned char)(col.y * 255);
	b = (unsigned char)(col.z * 255);

	/* place color */
	PIXEL(pos.x, pos.y) = RGBA(r, g, b, 255);
}

/* clear screen buffer */
void export_clearscreen(qcvm_t *qcvm)
{
	memset(pixels, 0, WIDTH * HEIGHT * sizeof(int));
}

/* make screen buffer visible */
void export_drawscreen(qcvm_t *qcvm)
{
	SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(int));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/* error */
void error(const char *err)
{
	fprintf(stderr, "ERROR: %s\n", err);
	exit(1);
}

/* main */
int main(int argc, char **argv)
{
	/* variables */
	qcvm_t *qcvm;
	int func_draw;
	int func_setup;
	int func_update;
	int func_shutdown;
	int running;
	SDL_Event event;

	/*
	 * startup
	 */

	/* load qcvm */
	qcvm = qcvm_open(PROGS);

	/* check validity */
	if (qcvm == NULL) error("Failed to load required QuakeC module!\n");

	/* install qclib */
	qclib_install(qcvm);

	/* install our own exports */
	qcvm_add_export(qcvm, export_drawpixel);
	qcvm_add_export(qcvm, export_clearscreen);
	qcvm_add_export(qcvm, export_drawscreen);

	/* get function handles */
	func_draw = qcvm_get_function(qcvm, "draw");
	func_setup = qcvm_get_function(qcvm, "setup");
	func_update = qcvm_get_function(qcvm, "update");
	func_shutdown = qcvm_get_function(qcvm, "shutdown");

	/* check validitiy */
	if (func_draw < 1) error("Failed to find required QuakeC function draw()!");
	if (func_setup < 1) error("Failed to find required QuakeC function setup()!");
	if (func_update < 1) error("Failed to find required QuakeC function update()!");
	if (func_shutdown < 1) error("Failed to find required QuakeC function shutdown()!");

	/* SDL */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) error("Failed to initialize SDL!");
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	/* check validity */
	if (window == NULL) error("SDL window is NULL!");
	if (renderer == NULL) error("SDL renderer is NULL!");
	if (texture == NULL) error("SDL texture is NULL!");

	/* allocate writeable pixels */
	pixels = malloc(WIDTH * HEIGHT * sizeof(int));

	/* call quakec setup function */
	qcvm_run(qcvm, func_setup);

	/*
	 * main loop
	 */

	running = 1;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = 0;
					break;

				default:
					break;
			}
		}

		/* call qc draw function */
		qcvm_set_parm_vector(qcvm, 0, WIDTH, HEIGHT, 0);
		qcvm_run(qcvm, func_draw);
	}

	/*
	 * shutdown
	 */

	/* call quakec shutdown function */
	qcvm_run(qcvm, func_shutdown);

	/* close qcvm */
	qcvm_close(qcvm);

	/* pixels */
	free(pixels);

	/* close sdl */
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_Quit();

	/* return success */
	return 0;
}

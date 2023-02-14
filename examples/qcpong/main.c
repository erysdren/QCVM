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
	SDL_Window *window;
	SDL_Texture *texture;
	SDL_Renderer *renderer;

	/*
	 * startup
	 */

	/* load qcvm */
	qcvm = qcvm_open("qcpong.dat");

	/* check validity */
	if (qcvm == NULL) error("Failed to load required QuakeC module!\n");

	/* install qclib */
	qclib_install(qcvm);

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
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	/* check validity */
	if (window == NULL) error("SDL window is NULL!");
	if (renderer == NULL) error("SDL renderer is NULL!");
	if (texture == NULL) error("SDL texture is NULL!");

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

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	/*
	 * shutdown
	 */

	/* call quakec shutdown function */
	qcvm_run(qcvm, func_shutdown);

	/* close qcvm */
	qcvm_close(qcvm);

	/* close sdl */
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_Quit();

	/* return success */
	return 0;
}

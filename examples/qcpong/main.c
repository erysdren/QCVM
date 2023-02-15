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

/* https://github.com/dhepper/font8x8 */
#include "font8x8_basic.h"

/* constants */
#define WIDTH 640
#define HEIGHT 480
#define TITLE "QCPONG"
#define PROGS "../examples/qcpong/qcpong.dat"
#define ASPECT1 (float)(WIDTH) / (float)(HEIGHT)
#define ASPECT2 (float)(HEIGHT) / (float)(WIDTH)

/* macros */
#define RGBA(r, g, b, a) (unsigned int)((r << 24) | (g << 16) | (b << 8) | a)
#define ARGB(r, g, b, a) (unsigned int)((a << 24) | (r << 16) | (g << 8) | b)
#define PIXEL(x, y) ((unsigned int *)pixels)[(unsigned int)(y) * WIDTH + (unsigned int)(x)]

/* globals */
void *pixels;
SDL_Window *window;
SDL_Texture *texture;
SDL_Renderer *renderer;

/* set memory area to a 32 bit value */
void *memset32(void *s, unsigned int c, size_t n)
{
	/* variables */
	unsigned int *dst = (unsigned int *)s;

	/* loop to set memory */
	while (n--) *dst++ = c;

	/* return pointer to memory */
	return s;
}

/* draw pixel on screen buffer */
void export_drawpixel(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3 pos, color;
	unsigned char r, g, b;

	/* get parms */
	pos = qcvm_get_parm_vector(qcvm, 0);
	color = qcvm_get_parm_vector(qcvm, 1);

	if (pos.x > WIDTH - 1 || pos.y > HEIGHT - 1) return;
	if (pos.x < 0 || pos.y < 0) return;

	/* convert color values */
	r = (unsigned char)(color.x * 255);
	g = (unsigned char)(color.y * 255);
	b = (unsigned char)(color.z * 255);

	/* place color */
	PIXEL(pos.x, pos.y) = RGBA(r, g, b, 255);
}

/* clear screen buffer */
void export_clearscreen(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3 color;
	unsigned char r, g, b;

	/* if the user defined a color */
	if (qcvm_get_argc(qcvm))
	{
		/* get parameters */
		color = qcvm_get_parm_vector(qcvm, 0);

		/* convert color values */
		r = (unsigned char)(color.x * 255);
		g = (unsigned char)(color.y * 255);
		b = (unsigned char)(color.z * 255);

		/* fancy memset */
		memset32(pixels, RGBA(r, g, b, 255), WIDTH * HEIGHT);
	}
	else
	{
		/* memset */
		memset(pixels, 0, WIDTH * HEIGHT * sizeof(int));
	}
}

/* make screen buffer visible */
void export_drawscreen(qcvm_t *qcvm)
{
	SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(int));
}

/* draw a rectangle */
void export_drawrectangle(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3 pos, size, color;
	float filled;
	int x, y;
	unsigned char r, g, b;

	/* get parms */
	pos = qcvm_get_parm_vector(qcvm, 0);
	size = qcvm_get_parm_vector(qcvm, 1);
	color = qcvm_get_parm_vector(qcvm, 2);
	filled = qcvm_get_parm_float(qcvm, 3);

	/* sanity checks */
	if (pos.x < 0 || pos.y < 0) return;
	if (pos.x > WIDTH || pos.y > HEIGHT) return;
	if (pos.x + size.x < 0 || pos.y + size.y < 0) return;
	if (pos.x + size.x > WIDTH || pos.y + size.y > HEIGHT) return;

	/* convert color values */
	r = (unsigned char)(color.x * 255);
	g = (unsigned char)(color.y * 255);
	b = (unsigned char)(color.z * 255);

	/* inefficient loop */
	for (y = (int)pos.y; y < (int)(pos.y + size.y); y++)
	{
		for (x = (int)pos.x; x < (int)(pos.x + size.x); x++)
		{
			PIXEL(x, y) = RGBA(r, g, b, 255);
		}
	}
}

/* render char to pixel buffer */
void draw_char(int x, int y, unsigned char r, unsigned char g, unsigned char b, char *bitmap)
{
	/* variables */
	int cx, cy;

	/* plot loop */
	for (cx = 0; cx < 8; cx++)
	{
		for (cy = 0; cy < 8; cy++)
		{
			if (x + cy > WIDTH || y + cx > HEIGHT) return;

			if (bitmap[cx] & 1 << cy)
				PIXEL(x + cy, y + cx) = RGBA(r, g, b, 255);
		}
	}
}

/* draw text on screen */
void export_drawtext(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3 pos, color;
	const char *string;
	int i;
	char c;
	unsigned char r, g, b;

	/* get parms */
	pos = qcvm_get_parm_vector(qcvm, 0);
	color = qcvm_get_parm_vector(qcvm, 1);
	string = qcvm_get_parm_string(qcvm, 2);

	/* convert color values */
	r = (unsigned char)(color.x * 255);
	g = (unsigned char)(color.y * 255);
	b = (unsigned char)(color.z * 255);

	/* plot loop */
	for (i = 0; i < strlen(string); i++)
	{
		c = string[i];
		draw_char((int)pos.x + (i * 8), (int)pos.y, r, g, b, font8x8_basic[c]);
	}
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
	SDL_Rect dst_rect;
	int window_x, window_y;

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
	qcvm_add_export(qcvm, export_drawrectangle);
	qcvm_add_export(qcvm, export_drawtext);

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
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

		/* update dst position */
		SDL_GetWindowSize(window, &window_x, &window_y);
		if (window_y < window_x && (window_y * ASPECT1) < window_x)
		{
			dst_rect.x = (window_x / 2) - ((window_y * ASPECT1) / 2);
			dst_rect.y = 0;
		}
		else if (window_x / window_y == ASPECT1)
		{
			dst_rect.x = 0;
			dst_rect.y = 0;
		}
		else
		{
			dst_rect.x = 0;
			dst_rect.y = (window_y / 2) - ((window_x * ASPECT2) / 2);
		}

		/* update dst size */
		if (window_y < window_x && (window_y * ASPECT1) < window_x)
		{
			dst_rect.w = window_y * ASPECT1;
			dst_rect.h = window_y;
		}
		else if ((window_x / window_y) == ASPECT1)
		{
			dst_rect.w = window_x;
			dst_rect.h = window_y;
		}
		else
		{
			dst_rect.w = window_x;
			dst_rect.h = window_x * ASPECT2;
		}

		/* copy texture */
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
		SDL_RenderPresent(renderer);
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

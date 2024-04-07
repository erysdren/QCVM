/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2023-2024 erysdren (it/she)
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
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

/* qcvm */
#include "qcvm.h"

/* qclib */
#include "qclib.h"

/* https://github.com/dhepper/font8x8 */
#include "font8x8_basic.h"

/* https://github.com/nothings/stb */
#include "stb_image_write.h"

/* constants */
#define WIDTH 640
#define HEIGHT 480
#define TITLE "QCPONG"
#define PROGS "qcpong.dat"
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
int running;

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
	qcvm_vec3_t pos, color;
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
	PIXEL(pos.x, pos.y) = ARGB(r, g, b, 255);
}

/* clear screen buffer */
void export_clearscreen(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3_t color;
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
		memset32(pixels, ARGB(r, g, b, 255), WIDTH * HEIGHT);
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
	qcvm_vec3_t pos, size, color;
	float filled;
	int x, y;
	unsigned char r, g, b;

	/* get parms */
	pos = qcvm_get_parm_vector(qcvm, 0);
	size = qcvm_get_parm_vector(qcvm, 1);
	color = qcvm_get_parm_vector(qcvm, 2);
	filled = qcvm_get_parm_float(qcvm, 3);

	/* convert color values */
	r = (unsigned char)(color.x * 255);
	g = (unsigned char)(color.y * 255);
	b = (unsigned char)(color.z * 255);

	/* inefficient loop */
	/* todo: use memset32 for this */
	for (y = (int)pos.y; y < (int)(pos.y + size.y); y++)
	{
		for (x = (int)pos.x; x < (int)(pos.x + size.x); x++)
		{
			if (x < 0 || y < 0) continue;
			if (x > WIDTH - 1 || y > HEIGHT - 1) continue;
			PIXEL(x, y) = ARGB(r, g, b, 255);
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
			if (x + cy > WIDTH - 1 || y + cx > HEIGHT - 1) return;

			if (bitmap[cx] & 1 << cy)
				PIXEL(x + cy, y + cx) = ARGB(r, g, b, 255);
		}
	}
}

/* draw text on screen */
void export_drawtext(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3_t pos, color;
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
		if (c < 0 || c > 127) continue;
		draw_char((int)pos.x + (i * 8), (int)pos.y, r, g, b, font8x8_basic[c]);
	}
}

/* exit game */
void export_exit(qcvm_t *qcvm)
{
	running = 0;
}

/* take a screenshot */
void export_screenshot(qcvm_t *qcvm)
{
	const char *filename;

	filename = qcvm_get_parm_string(qcvm, 0);

	stbi_write_png(filename, WIDTH, HEIGHT, 4, pixels, WIDTH * sizeof(int));
}

/* hard exit without freeing anything */
void error(const char *err)
{
	fprintf(stderr, "ERROR: %s\n", err);
	exit(1);
}

qcvm_export_t _export_drawpixel = { .func = export_drawpixel, .name = "drawpixel" };
qcvm_export_t _export_clearscreen = { .func = export_clearscreen, .name = "clearscreen" };
qcvm_export_t _export_drawscreen = { .func = export_drawscreen, .name = "drawscreen" };
qcvm_export_t _export_drawrectangle = { .func = export_drawrectangle, .name = "drawrectangle" };
qcvm_export_t _export_drawtext = { .func = export_drawtext, .name = "drawtext" };
qcvm_export_t _export_exit = { .func = export_exit, .name = "exit" };
qcvm_export_t _export_screenshot = { .func = export_screenshot, .name = "screenshot" };

/* main */
int main(int argc, char **argv)
{
	/* variables */
	qcvm_t *qcvm;
	int func_draw;
	int func_setup;
	int func_update;
	int func_shutdown;
	int func_input;
	int global_time;
	int global_fps;
	SDL_Event event;
	SDL_Rect dst;
	int window_x, window_y;
	Uint64 frame_start;
	Uint64 frame_end;
	float fps;

	/*
	 * startup
	 */

	/* load qcvm */
	qcvm = qcvm_from_file(PROGS);

	/* check validity */
	if (qcvm == NULL) error("Failed to load required QuakeC module!");

	/* install qclib */
	qclib_install(qcvm);

	/* install our own exports */
	qcvm_add_export(qcvm, &_export_drawpixel);
	qcvm_add_export(qcvm, &_export_clearscreen);
	qcvm_add_export(qcvm, &_export_drawscreen);
	qcvm_add_export(qcvm, &_export_drawrectangle);
	qcvm_add_export(qcvm, &_export_drawtext);
	qcvm_add_export(qcvm, &_export_exit);
	qcvm_add_export(qcvm, &_export_screenshot);

	/* get function handles */
	func_draw = qcvm_find_function(qcvm, "draw");
	func_setup = qcvm_find_function(qcvm, "setup");
	func_update = qcvm_find_function(qcvm, "update");
	func_shutdown = qcvm_find_function(qcvm, "shutdown");
	func_input = qcvm_find_function(qcvm, "input");

	/* get global handles */
	global_time = qcvm_find_global(qcvm, "time");
	global_fps = qcvm_find_global(qcvm, "fps");

	/* check validitiy */
	if (func_draw < 1) error("Failed to find required QuakeC function \"draw()\"!");
	if (func_setup < 1) error("Failed to find required QuakeC function \"setup()\"!");
	if (func_update < 1) error("Failed to find required QuakeC function \"update()\"!");
	if (func_shutdown < 1) error("Failed to find required QuakeC function \"shutdown()\"!");
	if (func_input < 1) error("Failed to find required QuakeC function \"input()\"!");
	if (global_time < 1) error("Failed to find required QuakeC global \"time\"!");
	if (global_fps < 1) error("Failed to find required QuakeC global \"fps\"!");

	/* SDL */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) error("Failed to initialize SDL!");
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	/* check validity */
	if (window == NULL) error("SDL window is NULL!");
	if (renderer == NULL) error("SDL renderer is NULL!");
	if (texture == NULL) error("SDL texture is NULL!");

	/* allocate writeable pixels */
	pixels = malloc(WIDTH * HEIGHT * sizeof(int));

	/* check validity */
	if (pixels == NULL) error("Failed to allocate pixels buffer!");

	/* call quakec setup function */
	qcvm_run(qcvm, func_setup);

	/*
	 * main loop
	 */

	running = 1;
	while (running)
	{
		/* get frame start time */
		frame_start = SDL_GetPerformanceCounter();

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = 0;
					break;

				case SDL_KEYDOWN:
					qcvm_set_parm_float(qcvm, 0, (float)event.key.keysym.scancode);
					qcvm_set_parm_float(qcvm, 1, 1);
					qcvm_run(qcvm, func_input);
					break;

				case SDL_KEYUP:
					qcvm_set_parm_float(qcvm, 0, (float)event.key.keysym.scancode);
					qcvm_set_parm_float(qcvm, 1, 0);
					qcvm_run(qcvm, func_input);
					break;

				default:
					break;
			}
		}

		/* update time global */
		qcvm_set_global_float(qcvm, global_time, (float)SDL_GetTicks() / 1000.0f);

		/* call qc draw function */
		qcvm_set_parm_vector(qcvm, 0, WIDTH, HEIGHT, 0);
		qcvm_run(qcvm, func_draw);

		/* update dst position */
		SDL_GetWindowSize(window, &window_x, &window_y);
		if (window_y < window_x && (window_y * ASPECT1) < window_x)
		{
			dst.x = (window_x / 2) - ((window_y * ASPECT1) / 2);
			dst.y = 0;
		}
		else if (window_x / window_y == ASPECT1)
		{
			dst.x = 0;
			dst.y = 0;
		}
		else
		{
			dst.x = 0;
			dst.y = (window_y / 2) - ((window_x * ASPECT2) / 2);
		}

		/* update dst size */
		if (window_y < window_x && (window_y * ASPECT1) < window_x)
		{
			dst.w = window_y * ASPECT1;
			dst.h = window_y;
		}
		else if ((window_x / window_y) == ASPECT1)
		{
			dst.w = window_x;
			dst.h = window_y;
		}
		else
		{
			dst.w = window_x;
			dst.h = window_x * ASPECT2;
		}

		/* copy texture */
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, &dst);
		SDL_RenderPresent(renderer);

		/* update fps global */
		frame_end = SDL_GetPerformanceCounter();
		fps = 1.0f / ((frame_end - frame_start) / (float)SDL_GetPerformanceFrequency());
		qcvm_set_global_float(qcvm, global_fps, fps);
	}

	/*
	 * shutdown
	 */

	/* call quakec shutdown function */
	qcvm_run(qcvm, func_shutdown);

	/* close qcvm */
	qcvm_free(qcvm);

	/* pixels */
	free(pixels);

	/* close sdl */
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_Quit();

	/* return success */
	return 0;
}

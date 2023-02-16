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

/* glfw */
#include <GLFW/glfw3.h>

/* qcvm */
#include "qcvm.h"

/* qclib */
#include "qclib.h"

/*
 * constants
 */

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "QCVM OpenGL Example"

/*
 * globals
 */

GLFWwindow *window;

/*
 * utilities
 */

/* hard exit error */
void error(const char *err)
{
	fprintf(stderr, "ERROR: %s\n", err);
	exit(1);
}

/*
 * main
 */

int main(int argc, char **argv)
{
	/* variables */
	qcvm_t *qcvm;

	/* init */
	glfwInit();

	/* create window */
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) error("GLFW window is NULL!");

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	/* opengl */
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	/* main loop */
	while (!glfwWindowShouldClose(window))
	{
		/* poll events */
		glfwPollEvents();

		/* if esc, close */
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	/* clean up */
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

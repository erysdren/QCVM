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
#include <stdarg.h>
#include <string.h>

/* eqcc */
#include "eqcc.h"

/*
 * globals
 */

/* help text */
static const char help_text[] = \
	"\nEQCC - Erysdren's QuakeC Compiler\n\n" \
	"Usage:\n\teqcc file.qc\n\n" \
	"Other Commands:\n\t-h, --help\n\n";

/*
 * functions
 */

/* load entire file into memory */
void *load_file(const char *filename, int *buffer_len)
{
	/* variables */
	FILE *file;
	void *buffer;
	int len_file;

	/* open file */
	file = fopen(filename, "rb");
	if (!file) return NULL;

	/* get file len */
	fseek(file, 0L, SEEK_END);
	len_file = ftell(file);
	fseek(file, 0L, SEEK_SET);

	/* allocate space and read file */
	buffer = malloc(len_file);
	fread(buffer, len_file, 1, file);

	/* close file */
	fclose(file);

	/* set file len */
	if (buffer_len) *buffer_len = len_file;

	/* return pointer to buffer */
	return buffer;
}

/* print formatted error and exit */
void error(const char *fmt, ...)
{
	/* variables */
	va_list ap;
	char str[128];

	/* do vargs */
	va_start(ap, fmt);
	vsnprintf(str, sizeof(str), fmt, ap);
	va_end(ap);

	/* print message */
	fprintf(stderr, "err: %s\n", str);

	/* exit with error code 1 */
	exit(1);
}

/* print help text */
void print_help_text()
{
	printf("%s", help_text);
	exit(0);
}

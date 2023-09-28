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
#include <string.h>
#include <stdlib.h>

/* qcvm */
#include "qcvm.h"
#include "qclib.h"

/*
 * constants
 */

const char qcpkg_footer_magic[4] = "QCVM";

/*
 * functions
 */

/* cat executable and progs */
void qcpkg_concat(const char *self_filename, const char *destination_filename, const char *progs_filename)
{
	/* variables */
	FILE *self, *destination, *progs;
	int len_self, len_progs;
	void *buffer;

	/* open files  */
	self = fopen(self_filename, "rb");
	destination = fopen(destination_filename, "wb");
	progs = fopen(progs_filename, "rb");

	/* get self size */
	fseek(self, 0L, SEEK_END);
	len_self = ftell(self);
	fseek(self, 0L, SEEK_SET);

	/* copy self */
	buffer = malloc(len_self);
	fread(buffer, len_self, 1, self);
	fwrite(buffer, len_self, 1, destination);
	free(buffer);

	/* get progs size */
	fseek(progs, 0L, SEEK_END);
	len_progs = ftell(progs);
	fseek(progs, 0L, SEEK_SET);

	/* copy progs */
	buffer = malloc(len_progs);
	fread(buffer, len_progs, 1, progs);
	fwrite(buffer, len_progs, 1, destination);
	free(buffer);

	/* write footer */
	fwrite(&len_progs, 4, 1, destination);
	fwrite(qcpkg_footer_magic, 4, 1, destination);

	/* close */
	fclose(progs);
	fclose(destination);
	fclose(self);
}

/* check if we're a bundle */
int qcpkg_check_packaged(const char *self_filename)
{
	/* variables */
	FILE *self;
	char magic[4];
	int len_progs;

	/* open */
	self = fopen(self_filename, "rb");
	if (!self) return 0;

	/* seek to header location */
	fseek(self, 0, SEEK_END);
	fseek(self, -8, SEEK_CUR);

	/* read values */
	fread(&len_progs, 4, 1, self);
	fread(magic, 4, 1, self);

	/* close file */
	fclose(self);

	/* check validity */
	if (memcmp(magic, qcpkg_footer_magic, 4) != 0)
		return 0;

	return len_progs;
}

/* run package */
void qcpkg_run_package(const char *self_filename, int len_progs)
{
	/* variables */
	FILE *self;
	qcvm_t *qcvm;
	int func_main;
	void *buffer;

	/* open self */
	self = fopen(self_filename, "rb");

	/* seek to progs */
	fseek(self, 0L, SEEK_END);
	fseek(self, -(len_progs + 8), SEEK_CUR);

	/* read progs */
	buffer = malloc(len_progs);
	fread(buffer, len_progs, 1, self);
	fclose(self);

	/* open qcvm */
	qcvm = qcvm_from_memory(buffer, len_progs);
	if (qcvm == NULL)
	{
		printf("QCVM Error: %s\n", qcvm_get_error());
		return;
	}

	/* free buffer */
	free(buffer);

	/* install qclib */
	qclib_install(qcvm);

	/* find main */
	func_main = qcvm_find_function(qcvm, "main");
	if (func_main < 1)
	{
		printf("No function \"main\"\n");
		return;
	}

	/* run progs */
	qcvm_run(qcvm, func_main);
}

static int global_argc = 0;
static char **global_argv = NULL;

/* check arg */
const char *get_arg(const char *pattern)
{
	int i;

	for (i = 0; i < global_argc; i++)
	{
		/* check if arg is there */
		if (strcmp(global_argv[i], pattern) != 0)
			continue;

		/* check if it has a value */
		if (global_argc > (i + 1))
			return global_argv[i + 1];
	}

	return NULL;
}

/* main */
int main(int argc, char **argv)
{
	/* variables */
	int len_progs;
	const char *self_filename;
	const char *destination_filename;
	const char *progs_filename;

	/* check if we're bundled */
	if ((len_progs = qcpkg_check_packaged(argv[0])))
	{
		qcpkg_run_package(argv[0], len_progs);
		return 0;
	}

	/* emit help */
	if (argc < 2)
	{
		printf("qcpkg options:\n--stub <stub file>\n--progs <progs file>\n--output <output file>\n");
		return 0;
	}

	/* init argc/argv */
	global_argc = argc;
	global_argv = argv;

	/* check stub */
	self_filename = get_arg("--stub");
	if (!self_filename) self_filename = argv[0];

	/* check progs */
	progs_filename = get_arg("--progs");
	if (!progs_filename)
	{
		fprintf(stderr, "Error: No progs specified\n");
		return 1;
	}

	/* check destination filename */
	destination_filename = get_arg("--output");
	if (!destination_filename)
	{
		fprintf(stderr, "Error: No destination filename specified\n");
		return 1;
	}

	/* do concat */
	qcpkg_concat(self_filename, destination_filename, progs_filename);

	return 0;
}

/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2023-2025 erysdren (it/its)
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
#include "qcpkg.h"

/*
 * functions
 */

/* cat executable and progs */
int qcpkg_concat(const char *stub_filename, const char *destination_filename, const char *progs_filename)
{
	/* variables */
	FILE *stub, *destination, *progs;
	int len_stub, len_progs;
	void *buffer;

	/* open files  */
	stub = fopen(stub_filename, "rb");
	destination = fopen(destination_filename, "wb");
	progs = fopen(progs_filename, "rb");
	if (!stub || !destination || !progs)
		return 0;

	/* get stub size */
	fseek(stub, 0L, SEEK_END);
	len_stub = ftell(stub);
	fseek(stub, 0L, SEEK_SET);

	/* copy stub */
	buffer = malloc(len_stub);
	fread(buffer, len_stub, 1, stub);
	fwrite(buffer, len_stub, 1, destination);
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
	fwrite(QCPKG_FOOTER_MAGIC, 4, 1, destination);

	/* close */
	fclose(progs);
	fclose(destination);
	fclose(stub);

	return 1;
}

/* check if we're a bundle */
int qcpkg_check_packaged(const char *stub_filename)
{
	/* variables */
	FILE *stub;
	qcpkg_footer_t footer;

	/* open */
	stub = fopen(stub_filename, "rb");
	if (!stub) return 0;

	/* seek to footer location */
	fseek(stub, 0, SEEK_END);
	fseek(stub, -sizeof(qcpkg_footer_t), SEEK_CUR);

	/* read footer */
	fread(&footer.len_progs, sizeof(int), 1, stub);
	fread(footer.magic, sizeof(char), 4, stub);

	/* close file */
	fclose(stub);

	/* check validity */
	if (memcmp(footer.magic, QCPKG_FOOTER_MAGIC, 4) != 0)
		return 0;

	return footer.len_progs;
}

/* run package */
void qcpkg_run_package(const char *stub_filename, int len_progs)
{
	/* variables */
	FILE *stub;
	qcvm_t *qcvm;
	int func_main;
	void *buffer;

	/* open stub */
	stub = fopen(stub_filename, "rb");

	/* seek to progs */
	fseek(stub, 0L, SEEK_END);
	fseek(stub, -(len_progs + sizeof(qcpkg_footer_t)), SEEK_CUR);

	/* read progs */
	buffer = malloc(len_progs);
	fread(buffer, len_progs, 1, stub);
	fclose(stub);

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
int check_arg(const char *pattern)
{
	int i;

	for (i = 0; i < global_argc; i++)
	{
		/* check if arg is there */
		if (strcmp(global_argv[i], pattern) == 0)
			return 1;
	}

	return 0;
}

/* get arg */
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

void print_help(void)
{
	printf("qcpkg options:\n--stub <stub file>\n--progs <progs file>\n--output <output file>\n");
}

/* main */
int main(int argc, char **argv)
{
	/* variables */
	int len_progs;
	const char *stub_filename;
	const char *destination_filename;
	const char *progs_filename;

	/* check if we're bundled */
	if ((len_progs = qcpkg_check_packaged(argv[0])))
	{
		qcpkg_run_package(argv[0], len_progs);
		return 0;
	}

	/* init argc/argv */
	global_argc = argc;
	global_argv = argv;

	/* emit help */
	if (argc < 2 || check_arg("--help"))
	{
		print_help();
		return 0;
	}

	/* check stub */
	stub_filename = get_arg("--stub");
	if (!stub_filename)
	{
		if (check_arg("--verbose"))
			fprintf(stderr, "Warning: No stub specified, assuming self\n");
		stub_filename = argv[0];
	}

	/* check progs */
	progs_filename = get_arg("--progs");
	if (!progs_filename)
	{
		fprintf(stderr, "Error: No progs specified\n");
		print_help();
		return 1;
	}

	/* check destination filename */
	destination_filename = get_arg("--output");
	if (!destination_filename)
	{
		fprintf(stderr, "Error: No destination filename specified\n");
		print_help();
		return 1;
	}

	/* do concat */
	if (!qcpkg_concat(stub_filename, destination_filename, progs_filename))
	{
		fprintf(stderr, "Error: Failed to make package\n");
		return 1;
	}
	else
	{
		printf("Successfully built QC package \"%s\"\n", destination_filename);
	}

	return 0;
}

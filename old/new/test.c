/*
MIT License

Copyright (c) 2023-2025 erysdren (it/its)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>

#include "qcvm.h"

/*
 *
 * utilities
 *
 */

#define ASIZE(a) (sizeof(a)/sizeof(a[0]))
#define UNUSED(a) ((void)(a))

/* print qcvm error and exit */
static void die(int r)
{
	fprintf(stderr, "qcvm: \"%s\"\n", qcvm_result_string(r));
	exit(EXIT_FAILURE);
}

/* load an entire file into memory */
static void *load_file(const char *filename, size_t *sz)
{
	void *buffer;
	size_t filesize;
	FILE *file;

	file = fopen(filename, "rb");
	if (!file) return NULL;
	fseek(file, 0L, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	buffer = calloc(1, filesize);
	fread(buffer, 1, filesize, file);
	fclose(file);

	if (sz) *sz = filesize;

	return buffer;
}

/*
 *
 * builtins
 *
 */

int _print(qcvm_t *qcvm)
{
	int argc, r, i;

	if ((r = qcvm_query_argument_count(qcvm, &argc)) != QCVM_OK)
		return r;

	for (i = 0; i < argc; i++)
	{
		const char *s = NULL;

		if ((r = qcvm_get_argument_string(qcvm, i, &s)) != QCVM_OK)
			return r;

		fprintf(stdout, s);
	}

	fflush(stdout);

	return QCVM_OK;
}

static const uint32_t max_entities = 64;
static uint32_t num_entities = 0;

int _spawn(qcvm_t *qcvm)
{
	qcvm_return_entity(qcvm, ++num_entities);
	return QCVM_OK;
}

int _getenv(qcvm_t *qcvm)
{
	int r;
	const char *name;

	if ((r = qcvm_get_argument_string(qcvm, 0, &name)) != QCVM_OK)
		return r;

	if ((r = qcvm_return_string(qcvm, getenv(name))) != QCVM_OK)
		return r;

	return QCVM_OK;
}

struct qcvm_builtin builtins[] = {
	{"print", _print},
	{"spawn", _spawn},
	{"getenv", _getenv}
};

/*
 *
 * state callback
 *
 */

int _state_callback(qcvm_t *qcvm, float frame, int function)
{
	UNUSED(qcvm);
	printf("STATE: frame=%f function=%d\n", frame, function);
	return QCVM_OK;
}

/*
 *
 * main
 *
 */

int main(int argc, char **argv)
{
	int r;
	size_t entity_fields, entity_size;
	qcvm_t *qcvm;

	qcvm = malloc(sizeof(qcvm_t));
	if (qcvm == NULL)
	{
		printf("Failed to allocate QCVM structure");
		return 1;
	}

	/* load progs */
	if (argc > 1)
		qcvm->progs = load_file(argv[1], &qcvm->len_progs);
	else
		qcvm->progs = load_file("qc_test1.dat", &qcvm->len_progs);

	/* allocate tempstrings buffer */
	qcvm->len_tempstrings = 4096;
	qcvm->tempstrings = malloc(qcvm->len_tempstrings);

	/* allocate entities buffer */
	qcvm->len_entities = max_entities * max_entities;
	qcvm->entities = malloc(qcvm->len_entities);

	/* setup builtins */
	qcvm->num_builtins = ASIZE(builtins);
	qcvm->builtins = builtins;

	/* set state callback */
	qcvm->state_callback = _state_callback;

	/* init qcvm */
	if ((r = qcvm_init(qcvm)) != QCVM_OK)
		die(r);

	/* query entity info */
	if ((r = qcvm_query_entity_info(qcvm, &entity_fields, &entity_size)) != QCVM_OK)
		die(r);

	/* realloc entities buffer if we need to */
	if (entity_size * max_entities > qcvm->len_entities)
	{
		qcvm->len_entities = entity_size * max_entities;
		qcvm->entities = realloc(qcvm->entities, qcvm->len_entities);
	}

	/* run main function */
	if ((r = qcvm_run(qcvm, "main")) != QCVM_OK)
		die(r);

	/* free data */
	free(qcvm->progs);
	free(qcvm->tempstrings);
	free(qcvm->entities);
	free(qcvm);

	return 0;
}

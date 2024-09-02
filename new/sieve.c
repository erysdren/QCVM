/*
MIT License

Copyright (c) 2023-2024 erysdren (it/she/they)

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
#include <time.h>

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

struct qcvm_builtin builtins[] = {
	{"print", _print}
};

/*
 *
 * main
 *
 */

int main(int argc, char **argv)
{
	qcvm_t *qcvm;
	double start, end, elapsed;
	float num_primes;
	int r;

	UNUSED(argc);
	UNUSED(argv);

	qcvm = calloc(1, sizeof(qcvm_t));
	if (!qcvm)
		return 1;

	/* load progs */
	qcvm->progs = load_file("sieve.dat", &qcvm->len_progs);

	/* setup builtins */
	qcvm->num_builtins = ASIZE(builtins);
	qcvm->builtins = builtins;

	/* init qcvm */
	if ((r = qcvm_init(qcvm)) != QCVM_OK)
		die(r);

	/* get start time */
	start = (double)clock() / (double)CLOCKS_PER_SEC;

	/* run sieve function */
	if ((r = qcvm_run(qcvm, "main")) != QCVM_OK)
		die(r);

	/* get return value */
	qcvm_get_return_float(qcvm, &num_primes);

	/* get end time */
	end = (double)clock() / (double)CLOCKS_PER_SEC;

	/* get elapsed time */
	elapsed = end - start;

	printf("SIEVE: num_primes=%f start=%f end=%f elapsed=%f\n", num_primes, start, end, elapsed);

	/* free data */
	free(qcvm->progs);
	free(qcvm);

	return 0;
}

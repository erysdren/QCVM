/*
MIT License

Copyright (c) 2023-2026 erysdren (it/its)

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

#include <qcvm/qcvm.h>

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

static const uint32_t max_entities = 64;
static uint32_t num_entities = 0;
static size_t entity_fields = 0;
static size_t entity_size = 0;

static int vm_spawn(qcvm_t *qcvm, void *user)
{
	qcvm_return_entity(qcvm, ++num_entities);
	return QCVM_OK;
}

static int vm_printf(qcvm_t *qcvm, void *user)
{
	const char *fmt;
	char c;
	int r;
	int argc, arg;
	int i;
	float f;
	float v[3];
	const char *s;

	if ((r = qcvm_query_argument_count(qcvm, &argc)) != QCVM_OK)
		return r;

	if ((r = qcvm_get_argument_string(qcvm, 0, &fmt)) != QCVM_OK)
		return r;

	arg = 1;
	while ((c = *fmt++))
	{
		if (arg > argc)
			break;

		if (c != '%')
		{
			fputc(c, stdout);
			continue;
		}

		switch ((c = *fmt++))
		{
			/* % */
			case '%':
				fputc(c, stdout);
				break;

			/* string */
			case 's':
				qcvm_get_argument_string(qcvm, arg, &s);
				fprintf(stdout, "%s", s);
				arg++;
				break;

			/* int */
			case 'd':
				qcvm_get_argument_int(qcvm, arg, &i);
				fprintf(stdout, "%d", i);
				arg++;
				break;

			/* float */
			case 'f':
				qcvm_get_argument_float(qcvm, arg, &f);
				fprintf(stdout, "%g", f);
				arg++;
				break;

			/* vector */
			case 'v':
				qcvm_get_argument_vector(qcvm, arg, &v[0], &v[1], &v[2]);
				fprintf(stdout, "%g %g %g", v[0], v[1], v[2]);
				arg++;
				break;
		}
	}

	fflush(stdout);

	return QCVM_OK;
}

static int vm_sprintf(qcvm_t *qcvm, void *user)
{
	static char buffer[1024];
	char *bufptr = buffer;
	const char *fmt;
	char c;
	int r;
	int argc, arg;
	int i;
	float f;
	float v[3];
	const char *s;

	if ((r = qcvm_query_argument_count(qcvm, &argc)) != QCVM_OK)
		return r;

	if ((r = qcvm_get_argument_string(qcvm, 0, &fmt)) != QCVM_OK)
		return r;

	arg = 1;
	while ((c = *fmt++))
	{
		if (arg > argc)
			break;

		if (c != '%')
		{
			*bufptr++ = c;
			continue;
		}

		switch ((c = *fmt++))
		{
			/* % */
			case '%':
				*bufptr++ = c;
				break;

			/* string */
			case 's':
				qcvm_get_argument_string(qcvm, arg, &s);
				bufptr += sprintf(bufptr, "%s", s);
				arg++;
				break;

			/* int */
			case 'd':
				qcvm_get_argument_int(qcvm, arg, &i);
				bufptr += sprintf(bufptr, "%d", i);
				arg++;
				break;

			/* float */
			case 'f':
				qcvm_get_argument_float(qcvm, arg, &f);
				bufptr += sprintf(bufptr, "%g", f);
				arg++;
				break;

			/* vector */
			case 'v':
				qcvm_get_argument_vector(qcvm, arg, &v[0], &v[1], &v[2]);
				bufptr += sprintf(bufptr, "%g %g %g", v[0], v[1], v[2]);
				arg++;
				break;
		}
	}

	*bufptr++ = '\0';

	qcvm_return_string(qcvm, buffer);

	return QCVM_OK;
}

struct qcvm_builtin builtins[] = {
	{"spawn", vm_spawn, NULL},
	{"printf", vm_printf, NULL},
	{"sprintf", vm_sprintf, NULL}
};

/*
 *
 * main
 *
 */

int main(int argc, char **argv)
{
	qcvm_t *qcvm;
	int r;

	UNUSED(argc);
	UNUSED(argv);

	qcvm = calloc(1, sizeof(qcvm_t));
	if (!qcvm)
		return 1;

	/* load progs */
	qcvm->progs = load_file("hello.dat", &qcvm->len_progs);

	/* setup builtins */
	qcvm->num_builtins = ASIZE(builtins);
	qcvm->builtins = builtins;

	/* setup tempstrings buffer */
	qcvm->tempstrings = calloc(1, 1024);
	qcvm->len_tempstrings = 1024;

	/* setup entities buffer */
	qcvm_query_entity_info(qcvm, &entity_fields, &entity_size);
	qcvm->entities = calloc(1, max_entities * entity_size);
	qcvm->len_entities = max_entities * entity_size;

	/* init qcvm */
	if ((r = qcvm_init(qcvm)) != QCVM_OK)
		die(r);

	/* run main function */
	if ((r = qcvm_run(qcvm, "main")) != QCVM_OK)
		die(r);

	/* free data */
	free(qcvm->entities);
	free(qcvm->tempstrings);
	free(qcvm->progs);
	free(qcvm);

	return 0;
}

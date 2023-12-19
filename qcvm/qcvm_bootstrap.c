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

/* qcvm */
#include "qcvm_private.h"

/*
 * globals
 */

enum
{
	QCVM_ERROR_NONE,
	QCVM_ERROR_FOPEN,
	QCVM_ERROR_MALLOC,
	QCVM_ERROR_VERSION,
	QCVM_ERROR_COMPRESSED
};

int qcvm_error = QCVM_ERROR_NONE;

const char *qcvm_error_strings[] = {
	"None",
	"Could not open file",
	"Could not allocate memory",
	"Invalid version in header",
	"Compressed progs are not supported"
};

/*
 * functions
 */

/* set error */
static void qcvm_set_error(int e)
{
	qcvm_error = e;
}

/* create qcvm from memory */
qcvm_t *qcvm_from_memory(void *memory, size_t size)
{
	/* variables */
	qcvm_t *qcvm;

	/* allocate qcvm */
	qcvm = calloc(1, sizeof(qcvm_t));
	if (qcvm == NULL)
	{
		qcvm_set_error(QCVM_ERROR_MALLOC);
		return NULL;
	}

	/* allocate pool */
	qcvm->len_pool = size;
	qcvm->pool = malloc(qcvm->len_pool);
	if (qcvm->pool == NULL)
	{
		qcvm_set_error(QCVM_ERROR_MALLOC);
		free(qcvm);
		return NULL;
	}

	/* copy in pool */
	memcpy(qcvm->pool, memory, size);

	/* check version */
	qcvm->header = (qcvm_header_t *)qcvm->pool;
	if (qcvm->header->version != PROGS_VERSION_ID && qcvm->header->version != PROGS_VERSION_FTE)
	{
		qcvm_set_error(QCVM_ERROR_VERSION);
		free(qcvm->pool);
		free(qcvm);
		return NULL;
	}

	if (qcvm->header->version == PROGS_VERSION_FTE)
	{
		/* check if its fte16 */
		if (qcvm->header->extended_version != PROGS_EXTENDED_VERSION_FTE16)
		{
			qcvm_set_error(QCVM_ERROR_VERSION);
			free(qcvm->pool);
			free(qcvm);
			return NULL;
		}

		/* check if its compressed*/
		if (qcvm->header->num_compressed_functions != 0)
		{
			qcvm_set_error(QCVM_ERROR_COMPRESSED);
			free(qcvm->pool);
			free(qcvm);
			return NULL;
		}
	}

	/* set pointer addresses */
	qcvm->functions = (qcvm_function_t *)((char *)qcvm->pool + qcvm->header->ofs_functions);
	qcvm->global_vars = (qcvm_var_t *)((char *)qcvm->pool + qcvm->header->ofs_global_vars);
	qcvm->field_vars = (qcvm_var_t *)((char *)qcvm->pool + qcvm->header->ofs_field_vars);
	qcvm->statements = (qcvm_statement_t *)((char *)qcvm->pool + qcvm->header->ofs_statements);
	qcvm->globals = (qcvm_global_t *)((char *)qcvm->pool + qcvm->header->ofs_globals);

	#if ALLOCATE_TEMPSTRINGS
	qcvm->strings = (char *)malloc(qcvm->header->len_strings + TEMPSTRINGS_SIZE);
	qcvm->tempstrings = (char *)qcvm->strings + qcvm->header->len_strings;
	qcvm->tempstrings_ptr = qcvm->tempstrings;
	memcpy(qcvm->strings, (char *)qcvm->pool + qcvm->header->ofs_strings, qcvm->header->len_strings);
	#else
	qcvm->strings = (char *)qcvm->pool + qcvm->header->ofs_strings;
	#endif

	#if ALLOCATE_ENTITIES
	qcvm->entities = malloc(sizeof(qcvm_entity_t) * NUM_ENTITIES);
	qcvm->num_entities = 0;
	#else
	qcvm->entities = NULL;
	qcvm->num_entities = -1;
	#endif

	#if ALLOCATE_EXPORTS
	qcvm->exports = malloc(sizeof(qcvm_export_t) * NUM_EXPORTS);
	qcvm->num_exports = 0;
	#else
	qcvm->exports = NULL;
	qcvm->num_exports = -1;
	#endif

	/* return pointer */
	return qcvm;
}

/* load progs.dat and return handle */
qcvm_t *qcvm_from_file(const char *filename)
{
	/* variables */
	qcvm_t *qcvm;
	FILE *file;
	size_t len_file;
	void *pool;

	/* open file pointer */
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		qcvm_set_error(QCVM_ERROR_FOPEN);
		return NULL;
	}

	/* get length of file */
	fseek(file, 0L, SEEK_END);
	len_file = ftell(file);
	fseek(file, 0L, SEEK_SET);

	/* malloc memory pool */
	pool = malloc(len_file);
	if (pool == NULL)
	{
		qcvm_set_error(QCVM_ERROR_MALLOC);
		fclose(file);
		return NULL;
	}

	/* read in pool */
	fread(pool, len_file, 1, file);

	/* create qcvm from the memory */
	qcvm = qcvm_from_memory(pool, len_file);

	/* free memory */
	free(pool);

	/* close file pointer */
	fclose(file);

	/* return pointer */
	return qcvm;
}

/* destroy qcvm context */
void qcvm_free(qcvm_t *qcvm)
{
	if (qcvm)
	{
		/* free memory pool */
		if (qcvm->pool) free(qcvm->pool);

		/* free string table */
		#if ALLOCATE_TEMPSTRINGS
		if (qcvm->strings) free(qcvm->strings);
		#endif

		/* free entity table */
		#if ALLOCATE_ENTITIES
		if (qcvm->entities) free(qcvm->entities);
		#endif

		/* free export table */
		#if ALLOCATE_EXPORTS
		if (qcvm->exports) free(qcvm->exports);
		#endif

		/* free struct */
		free(qcvm);
	}
}

/* get error */
const char *qcvm_get_error(void)
{
	return qcvm_error_strings[qcvm_error];
}

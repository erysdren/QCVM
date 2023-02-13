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

/* qclib */
#include "qclib.h"

/* load progs.dat and return handle */
qcvm_t *qcvm_open(const char *filename)
{
	/* variables */
	qcvm_t *qcvm;
	FILE *file;
	size_t len_file;

	/* open file pointer */
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		return NULL;
	}

	/* malloc qcvm struct */
	qcvm = malloc(sizeof(qcvm_t));
	if (qcvm == NULL)
	{
		fclose(file);
		return NULL;
	}

	/* get length of file */
	fseek(file, 0L, SEEK_END);
	len_file = ftell(file);
	fseek(file, 0L, SEEK_SET);

	/* malloc memory pool */
	qcvm->pool = malloc(len_file);
	if (qcvm->pool == NULL)
	{
		fclose(file);
		free(qcvm);
		return NULL;
	}

	/* read in pool */
	fread(qcvm->pool, len_file, 1, file);
	qcvm->len_pool = len_file;

	/* check header */
	qcvm->header = (qcvm_header_t *)qcvm->pool;
	if (qcvm->header->version != 6)
	{
		fclose(file);
		free(qcvm->pool);
		free(qcvm);
		return NULL;
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
	memcpy(qcvm->strings, (char *)qcvm->pool + qcvm->header->ofs_strings, qcvm->header->len_strings);
	#else
	qcvm->strings = (char *)qcvm->pool + qcvm->header->ofs_strings;
	#endif

	#if ALLOCATE_ENTITIES
	qcvm->entities = malloc(ENTITY_SIZE * NUM_ENTITIES);
	qcvm->num_entities = 0;
	#else
	qcvm->entities = NULL;
	qcvm->num_entities = -1;
	#endif

	#if ALLOCATE_EXPORTS
	qcvm->exports = malloc(sizeof(qcvm_export_t) * NUM_EXPORTS);
	#else
	qcvm->exports = NULL;
	#endif

	/* close file pointer */
	fclose(file);

	/* return pointer */
	return qcvm;
}

/* destroy qcvm context */
void qcvm_close(qcvm_t *qcvm)
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

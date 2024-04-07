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
#include <string.h>

/* qcvm */
#include "qcvm_private.h"

/*
 * globals
 */

/* set global entity by number */
void qcvm_set_global_entity(qcvm_t *qcvm, int global, int entity)
{
	GET_ENTITY(global) = entity * sizeof(qcvm_entity_t);
}

/* set global int by number */
void qcvm_set_global_int(qcvm_t *qcvm, int global, int val)
{
	GET_INT(global) = val;
}

/* set global float by number */
void qcvm_set_global_float(qcvm_t *qcvm, int global, float val)
{
	GET_FLOAT(global) = val;
}

/* set global vector by number */
void qcvm_set_global_vector(qcvm_t *qcvm, int global, float a, float b, float c)
{
	GET_FLOAT(global) = a;
	GET_FLOAT(global + 1) = b;
	GET_FLOAT(global + 2) = c;
}

/* retrieve global entity */
int qcvm_get_global_entity(qcvm_t *qcvm, int global)
{
	return GET_ENTITY(global) / sizeof(qcvm_entity_t);
}

/* retrieve global int */
int qcvm_get_global_int(qcvm_t *qcvm, int global)
{
	return GET_INT(global);
}

/* retrieve global float */
float qcvm_get_global_float(qcvm_t *qcvm, int global)
{
	return GET_FLOAT(global);
}

/* retrieve global vector */
qcvm_vec3_t qcvm_get_global_vector(qcvm_t *qcvm, int global)
{
	qcvm_vec3_t ret;

	ret.x = GET_FLOAT(global);
	ret.y = GET_FLOAT(global + 1);
	ret.z = GET_FLOAT(global + 2);

	return ret;
}

/* find global by name */
int qcvm_find_global(qcvm_t *qcvm, const char *name)
{
	/* variables */
	int i;

	/* search globals */
	for (i = 1; i < qcvm->header->num_global_vars; i++)
	{
		if (strcmp(GET_STRING_OFS(qcvm->global_vars[i].name), name) == 0)
			return qcvm->global_vars[i].ofs;
	}

	/* return failure */
	return -1;
}

/* find global entity by type */
int qcvm_find_global_by_type(qcvm_t *qcvm, const char *name, qcvm_export_type_t type)
{
	int i;

	for (i = 1; i < qcvm->header->num_global_vars; i++)
	{
		if ((qcvm->global_vars[i].type & ~32768) == type
			&& strcmp(name, GET_STRING_OFS(qcvm->global_vars[i].name)) == 0)
			return i;
	}

	return -1;
}

/* find global float by name */
int qcvm_find_global_float(qcvm_t *qcvm, const char *name)
{
	return qcvm_find_global_by_type(qcvm, name, QCVM_FLOAT);
}

/* find global int by name */
int qcvm_find_global_int(qcvm_t *qcvm, const char *name)
{
	return qcvm_find_global_by_type(qcvm, name, QCVM_INT);
}

/* find global entity by name */
int qcvm_find_global_entity(qcvm_t *qcvm, const char *name)
{
	return qcvm_find_global_by_type(qcvm, name, QCVM_ENTITY);
}

/* find global string by name */
int qcvm_find_global_string(qcvm_t *qcvm, const char *name)
{
	return qcvm_find_global_by_type(qcvm, name, QCVM_STRING);
}

/* find global vector by name */
int qcvm_find_global_vector(qcvm_t *qcvm, const char *name)
{
	return qcvm_find_global_by_type(qcvm, name, QCVM_VECTOR);
}

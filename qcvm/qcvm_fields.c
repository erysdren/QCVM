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
 * fields
 */

/* get entity field string */
const char *qcvm_get_field_string(qcvm_t *qcvm, int entity, int field)
{
	return GET_STRING_OFS(FIELD_INT(entity, field));
}

/* get entity field vector */
qcvm_vec3_t qcvm_get_field_vector(qcvm_t *qcvm, int entity, int field)
{
	qcvm_vec3_t ret;

	ret.x = FIELD_FLOAT(entity, field);
	ret.y = FIELD_FLOAT(entity, field + 1);
	ret.z = FIELD_FLOAT(entity, field + 2);

	return ret;
}

/* get entity field float */
float qcvm_get_field_float(qcvm_t *qcvm, int entity, int field)
{
	return FIELD_FLOAT(entity, field);
}

/* get entity field int */
int qcvm_get_field_int(qcvm_t *qcvm, int entity, int field)
{
	return FIELD_INT(entity, field);
}

/* set entity field vector */
void qcvm_set_field_vector(qcvm_t *qcvm, int entity, int field, float a, float b, float c)
{
	FIELD_FLOAT(entity, field) = a;
	FIELD_FLOAT(entity, field + 1) = b;
	FIELD_FLOAT(entity, field + 2) = c;
}

/* set entity field float */
void qcvm_set_field_float(qcvm_t *qcvm, int entity, int field, float val)
{
	FIELD_FLOAT(entity, field) = val;
}

/* set entity field int */
void qcvm_set_field_int(qcvm_t *qcvm, int entity, int field, int val)
{
	FIELD_INT(entity, field) = val;
}

/* find entity field by name */
int qcvm_find_field(qcvm_t *qcvm, const char *name)
{
	/* variables */
	int i;

	/* search globals */
	for (i = 1; i < qcvm->header->num_field_vars; i++)
	{
		if (strcmp(GET_STRING_OFS(qcvm->field_vars[i].name), name) == 0)
			return qcvm->field_vars[i].ofs;
	}

	/* return failure */
	return -1;
}

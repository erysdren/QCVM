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
 * set parameters
 */

/* set string parameter */
void qcvm_set_parm_string(qcvm_t *qcvm, int parm, const char *s)
{
	/* bounds check */
	if (qcvm->tempstrings_ptr + strlen(s) + 1 > qcvm->tempstrings + TEMPSTRINGS_SIZE)
		qcvm->tempstrings_ptr = qcvm->tempstrings;

	/* sprintf */
	sprintf(qcvm->tempstrings_ptr, "%s", s);

	/* return str */
	GET_INT(OFS_PARM0 + (parm * 3)) = (qcvm->tempstrings_ptr) - qcvm->strings;

	/* advance ptr */
	qcvm->tempstrings_ptr += strlen(s) + 1;
}

/* set vector parameter */
void qcvm_set_parm_vector(qcvm_t *qcvm, int parm, float a, float b, float c)
{
	GET_FLOAT(OFS_PARM0 + (parm * 3)) = a;
	GET_FLOAT(OFS_PARM0 + (parm * 3) + 1) = b;
	GET_FLOAT(OFS_PARM0 + (parm * 3) + 2) = c;
}

/* set integer parameter */
void qcvm_set_parm_int(qcvm_t *qcvm, int parm, int val)
{
	GET_INT(OFS_PARM0 + (parm * 3)) = val;
}

/* set float parameter */
void qcvm_set_parm_float(qcvm_t *qcvm, int parm, float val)
{
	GET_FLOAT(OFS_PARM0 + (parm * 3)) = val;
}

/*
 * get parameters
 */

/* get argc */
int qcvm_get_argc(qcvm_t *qcvm)
{
	return qcvm->function_argc;
}

/* get entity parameter */
int qcvm_get_parm_entity(qcvm_t *qcvm, int parm)
{
	return GET_ENTITY(OFS_PARM0 + (parm * 3)) / sizeof(qcvm_entity_t);
}

/* get vector parameter */
qcvm_vec3_t qcvm_get_parm_vector(qcvm_t *qcvm, int parm)
{
	qcvm_vec3_t ret;

	ret.x = GET_FLOAT(OFS_PARM0 + (parm * 3));
	ret.y = GET_FLOAT(OFS_PARM0 + (parm * 3) + 1);
	ret.z = GET_FLOAT(OFS_PARM0 + (parm * 3) + 2);

	return ret;
}

/* get string parameter */
const char *qcvm_get_parm_string(qcvm_t *qcvm, int parm)
{
	return GET_STRING(OFS_PARM0 + (parm * 3));
}

/* get integer parameter */
int qcvm_get_parm_int(qcvm_t *qcvm, int parm)
{
	return GET_INT(OFS_PARM0 + (parm * 3));
}

/* get float parameter */
float qcvm_get_parm_float(qcvm_t *qcvm, int parm)
{
	return GET_FLOAT(OFS_PARM0 + (parm * 3));
}

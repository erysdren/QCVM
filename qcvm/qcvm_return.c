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

/* qcvm */
#include "qcvm_private.h"

/*
 * return values
 */

/* return a new entity */
void qcvm_return_entity(qcvm_t *qcvm, int entity)
{
	RETURN_ENTITY(&qcvm->entities[entity]);
}

/* return a string to the previous function */
void qcvm_return_string(qcvm_t *qcvm, const char *s)
{
	/* bounds check */
	if (qcvm->tempstrings_ptr + strlen(s) + 1 > qcvm->tempstrings + TEMPSTRINGS_SIZE)
		qcvm->tempstrings_ptr = qcvm->tempstrings;

	/* sprintf */
	sprintf(qcvm->tempstrings_ptr, "%s", s);

	/* return str */
	RETURN_STRING(qcvm->tempstrings_ptr);

	/* advance ptr */
	qcvm->tempstrings_ptr += strlen(s) + 1;
}

/* return a vector to the previous function */
void qcvm_return_vector(qcvm_t *qcvm, float a, float b, float c)
{
	RETURN_VECTOR(a, b, c);
}

/* return a int to the previous function */
void qcvm_return_int(qcvm_t *qcvm, int val)
{
	RETURN_INT(val);
}

/* return a float to the previous function */
void qcvm_return_float(qcvm_t *qcvm, float val)
{
	RETURN_FLOAT(val);
}

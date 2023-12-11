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
#include <stdarg.h>

/* include public header */
#include "qcvm.h"

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
	/* sprintf */
	memset(qcvm->tempstrings_ptr, 0, TEMPSTRINGS_SIZE);
	memcpy(qcvm->tempstrings_ptr, s, strlen(s));

	/* return str */
	RETURN_STRING(qcvm->tempstrings_ptr);
}

/* return a formatted string to the previous function */
void qcvm_return_stringf(qcvm_t *qcvm, const char *s, ...)
{
	/* variables */
	static char buffer[1024];
	va_list ap;

	/* do vargs */
	va_start(ap, s);
	vsnprintf(buffer, 1024, s, ap);
	va_end(ap);

	/* return string */
	qcvm_return_string(qcvm, buffer);
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

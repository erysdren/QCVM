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

/* qclib */
#include "qclib.h"

/* print text */
void qclib_print(qcvm_t *qcvm)
{
	int i;

	for (i = 0; i < qcvm->function_argc; i++)
	{
		printf("%s", GET_STRING(OFS_PARM0 + i * 3));
	}
}

/* spawn entity */
void qclib_spawn(qcvm_t *qcvm)
{
	RETURN_ENTITY(&qcvm->entities[qcvm->num_entities]);
	qcvm->num_entities++;
}

/* get length of string */
void qclib_strlen(qcvm_t *qcvm)
{
	RETURN_FLOAT((float)strlen(qcvm_get_parm_string(qcvm, 0)));
}

/* install qclib default builtin functions */
void qclib_install(qcvm_t *qcvm)
{
	qcvm->exports[0] = NULL;
	qcvm->exports[1] = qclib_print;
	qcvm->exports[2] = qclib_spawn;
	qcvm->exports[3] = qclib_strlen;

	qcvm->num_exports = 3;
}

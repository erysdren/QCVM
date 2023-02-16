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

/* return a concat of two strings */
void qclib_strcat(qcvm_t *qcvm)
{
	/* variables */
	const char *s[8];
	char buf[512];
	char *ptr;
	int i;

	ptr = buf;

	/* print strings */
	for (i = 0; i < qcvm_get_argc(qcvm); i++)
	{
		s[i] = qcvm_get_parm_string(qcvm, i);
		sprintf(ptr, "%s", s[i]);
		ptr += strlen(s[i]);
	}

	qcvm_return_string(qcvm, buf);
}

/* float to string */
void qclib_ftos(qcvm_t *qcvm)
{
	/* variables */
	float f;
	char buf[64];

	/* get parms */
	f = qcvm_get_parm_float(qcvm, 0);

	/* sprintf */
	if (f == (int)f)
		sprintf(buf, "%d", (int)f);
	else
		sprintf(buf, "%0.4f", f);

	/* return */
	qcvm_return_string(qcvm, buf);
}

/* vector to string */
void qclib_vtos(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3 v;
	char buf[64];

	/* get parms */
	v = qcvm_get_parm_vector(qcvm, 0);

	/* sprintf */
	sprintf(buf, "%0.4f %0.4f %0.4f", v.x, v.y, v.z);

	/* return */
	qcvm_return_string(qcvm, buf);
}

/* install qclib default builtin functions */
void qclib_install(qcvm_t *qcvm)
{
	qcvm->exports[0] = NULL;
	qcvm->exports[1] = qclib_print;
	qcvm->exports[2] = qclib_spawn;
	qcvm->exports[3] = qclib_strlen;
	qcvm->exports[4] = qclib_strcat;
	qcvm->exports[5] = qclib_ftos;
	qcvm->exports[6] = qclib_vtos;

	qcvm->num_exports = 6;
}

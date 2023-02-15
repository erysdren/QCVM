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
#include <string.h>

/* qcvm */
#include "qcvm_private.h"

/* qclib */
#include "qclib.h"

/*
 * set parameters
 */

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

/* get vector parameter */
qcvm_vec3 qcvm_get_parm_vector(qcvm_t *qcvm, int parm)
{
	qcvm_vec3 ret;

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

/*
 * return values 
 */

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

/*
 * exports
 */

/* add export */
void qcvm_add_export(qcvm_t *qcvm, qcvm_export_t export)
{
	qcvm->num_exports++;
	qcvm->exports[qcvm->num_exports] = export;
}

/*
 * misc
 */

/* get function by name search */
int qcvm_get_function(qcvm_t *qcvm, const char *name)
{
	/* variables */
	int i;

	/* loop through functions */
	for (i = 1; i < qcvm->header->num_functions; i++)
	{
		if (strcmp(name, GET_STRING_OFS(qcvm->functions[i].name)) == 0)
			return i;
	}

	/* return failure */
	return 0;
}

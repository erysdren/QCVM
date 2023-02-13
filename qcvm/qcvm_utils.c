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

/* add export */
void qcvm_add_export(qcvm_t *qcvm, qcvm_export_t export)
{
	qcvm->num_exports++;
	qcvm->exports[qcvm->num_exports] = export;
}

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

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
 * entities
 */

/* add entity and return id */
int qcvm_add_entity(qcvm_t *qcvm)
{
	qcvm->num_entities++;
	return qcvm->num_entities - 1;
}

/* find global entity by name */
int qcvm_find_entity(qcvm_t *qcvm, const char *name)
{
	int i;

	for (i = 1; i < qcvm->header->num_global_vars; i++)
	{
		if (qcvm->global_vars[i].type == 32772 && strcmp(name, GET_STRING_OFS(qcvm->global_vars[i].name)) == 0)
			return i;
	}

	return -1;
}

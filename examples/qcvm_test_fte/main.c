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
#include <stdlib.h>
#include <string.h>

/* qcvm */
#include "qcvm.h"

/* qclib */
#include "qclib.h"

/* main */
int main(int argc, char **argv)
{
	/* variables */
	qcvm_t *qcvm;

	/* load qcvm */
	qcvm = qcvm_from_file("test2.dat");

	/* check validity */
	if (qcvm == NULL)
	{
		fprintf(stderr, "failed to load test2.dat\n");
		return 1;
	}
	else
	{
		fprintf(stdout, "test2.dat successfully loaded\n");
	}

	/* install qclib */
	qclib_install(qcvm);

	/* run function main() */
	qcvm_run(qcvm, qcvm_find_function(qcvm, "main"));

	/* close qcvm */
	qcvm_free(qcvm);

	/* return success */
	return 0;
}

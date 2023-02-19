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
	int qcvm_field_field1;
	int qcvm_field_field2;
	int qcvm_entity_gtest;
	int qcvm_global_gtest;
	float field1;

	/* load qcvm */
	qcvm = qcvm_open("../qc/testqc.dat");

	/* check validity */
	if (qcvm == NULL)
		fprintf(stderr, "oh noes!\n");
	else
		fprintf(stdout, "progs.dat successfully loaded\n");

	/* install qclib */
	qclib_install(qcvm);

	/* run function test() */
	qcvm_run(qcvm, qcvm_find_function(qcvm, "test"));

	/* entity fields test */
	qcvm_field_field1 = qcvm_find_field(qcvm, "field1");
	qcvm_field_field2 = qcvm_find_field(qcvm, "field2");
	qcvm_global_gtest = qcvm_find_global(qcvm, "gtest");
	qcvm_entity_gtest = qcvm_get_global_entity(qcvm, qcvm_global_gtest);
	field1 = qcvm_get_field_float(qcvm, qcvm_entity_gtest, qcvm_field_field1);

	printf("%s\n", qcvm_get_field_string(qcvm, qcvm_entity_gtest, qcvm_field_field2));

	/* close qcvm */
	qcvm_close(qcvm);

	/* return success */
	return 0;
}

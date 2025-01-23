/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2023-2025 erysdren (it/its)
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

/* qcvm */
#include "qcvm.h"

/* qclib */
#include "qclib.h"

/*
 * globals
 */

qcvm_t *qcvm_module1;
qcvm_t *qcvm_module2;
int func_module1_main;
int func_module2_main;
int func_module1_recieve;
int func_module2_recieve;

/*
 * exports
 */

void export_sendstring(qcvm_t *qcvm)
{
	const char *s = qcvm_get_parm_string(qcvm, 0);

	if (qcvm == qcvm_module1)
	{
		qcvm_set_parm_string(qcvm_module2, 0, s);
		qcvm_set_parm_float(qcvm_module2, 1, 0);
		qcvm_run(qcvm_module2, func_module2_recieve);
	}
	else if (qcvm == qcvm_module2)
	{
		qcvm_set_parm_string(qcvm_module1, 0, s);
		qcvm_set_parm_float(qcvm_module1, 1, 0);
		qcvm_run(qcvm_module1, func_module1_recieve);
	}
}

qcvm_export_t _export_sendstring =
{
	.name = "sendstring",
	.func = export_sendstring,
	.type = QCVM_VOID,
	.argc = 1,
	.args[0] = {.name = "s", .type = QCVM_STRING}
};

void export_sendfloat(qcvm_t *qcvm)
{
	float f = qcvm_get_parm_float(qcvm, 0);

	if (qcvm == qcvm_module1)
	{
		qcvm_set_parm_float(qcvm_module2, 0, f);
		qcvm_set_parm_float(qcvm_module2, 1, 1);
		qcvm_run(qcvm_module2, func_module2_recieve);
	}
	else if (qcvm == qcvm_module2)
	{
		qcvm_set_parm_float(qcvm_module1, 0, f);
		qcvm_set_parm_float(qcvm_module1, 1, 1);
		qcvm_run(qcvm_module1, func_module1_recieve);
	}
}

qcvm_export_t _export_sendfloat =
{
	.name = "sendfloat",
	.func = export_sendfloat,
	.type = QCVM_VOID,
	.argc = 1,
	.args[0] = {.name = "f", .type = QCVM_FLOAT}
};

/*
 * main
 */

int main(int argc, char **argv)
{
	/* load modules */
	qcvm_module1 = qcvm_from_file("module1.dat");
	qcvm_module2 = qcvm_from_file("module2.dat");

	/* install qclib */
	qclib_install(qcvm_module1);
	qclib_install(qcvm_module2);

	/* add our exports */
	qcvm_add_export(qcvm_module1, &_export_sendstring);
	qcvm_add_export(qcvm_module1, &_export_sendfloat);
	qcvm_add_export(qcvm_module2, &_export_sendstring);
	qcvm_add_export(qcvm_module2, &_export_sendfloat);

	/* find functions */
	func_module1_main = qcvm_find_function(qcvm_module1, "module_main");
	func_module2_main = qcvm_find_function(qcvm_module2, "module_main");
	func_module1_recieve = qcvm_find_function(qcvm_module1, "module_recieve");
	func_module2_recieve = qcvm_find_function(qcvm_module2, "module_recieve");

	/* run mains */
	qcvm_run(qcvm_module1, func_module1_main);
	qcvm_run(qcvm_module2, func_module2_main);

	/* close modules */
	qcvm_free(qcvm_module1);
	qcvm_free(qcvm_module2);

	/* return */
	return 0;
}

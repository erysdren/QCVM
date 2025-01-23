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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* qcvm */
#include "qcvm_private.h"

/* find export by name */
int qcvm_find_export(qcvm_t *qcvm, const char *name)
{
	int i;

	for (i = 0; i < qcvm->num_exports; i++)
	{
		if (strcmp(name, qcvm->exports[i].name) == 0)
			return i;
	}

	return -1;
}

/* setup function */
int qcvm_function_setup(qcvm_t *qcvm, qcvm_function_t *func)
{
	/* variables */
	int i, x, p;

	/* sanity check */
	if (!qcvm || !func) return -1;

	/* setup stack */
	qcvm->stack[qcvm->stack_depth] = qcvm->xstack;
	qcvm->stack_depth++;

	/* check for stack overflow */
	if (qcvm->stack_depth >= STACK_DEPTH) return -1;

	/* check for stack overflow */
	if (qcvm->local_stack_used + func->num_locals > LOCAL_STACK_DEPTH) return -1;

	/* setup current local stack */
	for (i = 0; i < func->num_locals; i++)
	{
		qcvm->local_stack[qcvm->local_stack_used + i] =
			((int *)qcvm->globals)[func->first_parm + i];
	}

	/* add current stuff to the local stack */
	qcvm->local_stack_used += func->num_locals;

	/* copy parameters */
	p = func->first_parm;
	for (i = 0; i < func->num_parms; i++)
	{
		for (x = 0; x < func->parm_sizes[i]; x++)
		{
			((int *)qcvm->globals)[p] = ((int *)qcvm->globals)[OFS_PARM0 + i * 3 + x];
			p++;
		}
	}

	/* backup stack function */
	qcvm->xstack.function = func;

	/* return first statement */
	return func->first_statement - 1;
}

/* close current function */
int qcvm_function_close(qcvm_t *qcvm)
{
	/* variables */
	int i, num_locals;

	/* check for stack underflow */
	if (qcvm->stack_depth <= 0) return -2;

	num_locals = qcvm->xstack.function->num_locals;
	qcvm->local_stack_used -= num_locals;

	/* check for stack underflow */
	if (qcvm->local_stack_used < 0) return -1;

	/* restore locals from the stack */
	for (i = 0; i < num_locals; i++)
	{
		((int *)qcvm->globals)[qcvm->xstack.function->first_parm + i] =
			qcvm->local_stack[qcvm->local_stack_used + i];
	}

	/* move down the stack */
	qcvm->stack_depth--;
	qcvm->xstack.function = qcvm->stack[qcvm->stack_depth].function;

	/* return next statement */
	return qcvm->stack[qcvm->stack_depth].statement;
}

/* enable qcvm runtime, starting from func */
void qcvm_run(qcvm_t *qcvm, int func)
{
	/* sanity check */
	if (!qcvm || func <= 0 || func >= qcvm->header->num_functions)
		return;

	/* reset */
	qcvm->fail = 0;
	qcvm->done = 0;

	/* assign working function */
	qcvm->function_p = &qcvm->functions[func];
	qcvm->exit_depth = qcvm->stack_depth;

	/* enter function */
	qcvm->statement_i = qcvm_function_setup(qcvm, qcvm->function_p);

	/*
	 *
	 * main execution loop
	 *
	 * processes all opcodes recursively, until function tree finishes.
	 *
	 */
	while (1)
	{
		/* advance statement */
		qcvm->statement_i++;

		/* get current statement and evaluators */
		qcvm->statement_p = &qcvm->statements[qcvm->statement_i];
		qcvm->eval_p[1] = (qcvm_evaluator_t *)&qcvm->globals[qcvm->statement_p->vars[0]];
		qcvm->eval_p[2] = (qcvm_evaluator_t *)&qcvm->globals[qcvm->statement_p->vars[1]];
		qcvm->eval_p[3] = (qcvm_evaluator_t *)&qcvm->globals[qcvm->statement_p->vars[2]];

		/* update stack */
		qcvm->xstack.function->profile++;
		qcvm->xstack.statement = qcvm->statement_i;

		/* check opcode validity */
		if (qcvm->statement_p->opcode >= OPCODE_MAX)
		{
			fprintf(stderr, "opcode %u is out of range!", qcvm->statement_p->opcode);
			qcvm->fail = 1;
			return;
		}

		/* process opcode */
		qcvm_opcode_table[qcvm->statement_p->opcode].func(qcvm);

		/* check for fail or done */
		if (qcvm->fail || qcvm->done)
			return;
	}
}

/******************************************************************************
 *
 * QCVM is Copyright (C) 2023-2024 erysdren (it/she)
 * Quake is Copyright (C) 1996-1997 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Project:			QCVM
 *
 * License:			GNU GPL v2
 *
 * Authors:			erysdren (it/she)
 *
 * File:			/source/qcvm.c
 *
 * Description:		QCVM main runtime
 *
 *****************************************************************************/

/*
 * headers
 */

/* std */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* qcvm */
#include "qcvm.h"
#include "libqcvm.h"

/*
 * globals
 */

/* main */
qc_t *qc;
qc_function_t *qc_functions;
char *qc_strings;
qc_def_t *qc_fielddefs;
qc_def_t *qc_globaldefs;
qc_statement_t *qc_statements;
float *qc_globals;
int qc_edict_size;

/* stack */
qc_stack_t qc_stack[QC_MAX_STACK_DEPTH];
int qc_depth;
int qc_localstack[QC_LOCALSTACK_SIZE];
int qc_localstack_used;

/* trace */
int qc_trace;

/* x */
qc_function_t *qc_xfunction;
int qc_xstatement;

/* args */
int qc_argc;

/* builtins */
qc_builtin_t *qc_builtins = qcvm_builtins;
/* int qc_num_builtins = qcvm_num_builtins; */

/*
 * functions
 */

/* load qc progs into memory */
int qc_load(const char *filename)
{
	/* variables */
	FILE *file;
	size_t filesize;

	/* open file */
	file = fopen(filename, "rb");
	if (!file)
	{
		qc_error("qc_load(): couldn't open file");
		return 1;
	}

	/* get length of file */
	fseek(file, 0L, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	/* allocate qc object */
	qc = (qc_t *)malloc(filesize);

	/* read in the whole file */
	fread(qc, filesize, 1, file);

	/* check version */
	if (qc->version != QC_VERSION)
	{
		qc_error("qc_load(): invalid progs version %d, should be %d", qc->version, QC_VERSION);
		return 2;
	}

	/* setup pointers */
	qc_functions = (qc_function_t *)((uint8_t *)qc + qc->ofs_functions);
	qc_strings = (char *)qc + qc->ofs_strings;
	qc_globaldefs = (qc_def_t *)((uint8_t *)qc + qc->ofs_globaldefs);
	qc_fielddefs = (qc_def_t *)((uint8_t *)qc + qc->ofs_fielddefs);
	qc_statements = (qc_statement_t *)((uint8_t *)qc + qc->ofs_statements);
	qc_globals = (float *)((uint8_t *)qc + qc->ofs_globals);

	/*
	qc_global_struct = (globalvars_t *)((uint8_t *)qc + qc->ofs_globals);
	qc_globals = (float *)qc_global_struct;
	*/

	/* return success */
	return 0;
}

/* free qc memory */
void qc_exit()
{
	if (qc) free(qc);
}

/* aborts the current function and returns an error */
void qc_error(char *s, ...)
{
	/* variables */
	va_list args;
	char formatted[1024];

	/* print vargs */
	va_start(args, s);
	vsprintf(formatted, s, args);
	va_end (args);

	/* print error message */
	fprintf(stderr, "%s\n", formatted);

	/* dump the stack */
	qc_depth = 0;

	/* stop qc from functioning */
	qc_exit();

	/* just exit i guess? */
	exit(EXIT_FAILURE);
}

/* make this the currently executing function */
int qc_function_enter(qc_function_t *f)
{
	/* variables */
	int i, j, c, o;

	/* setup stack */
	qc_stack[qc_depth].s = qc_xstatement;
	qc_stack[qc_depth].f = qc_xfunction;

	qc_depth++;

	/* check for stack overflow */
	if (qc_depth >= QC_MAX_STACK_DEPTH)
		qc_error("qc_function_enter(): stack overflow");

	/* save off any locals that the new function steps on */
	c = f->locals;

	/* check for stack overflow */
	if (qc_localstack_used + c > QC_LOCALSTACK_SIZE)
		qc_error("qc_function_enter(): locals stack overflow\n");

	/* set the local stack */
	for (i = 0; i < c; i++)
		qc_localstack[qc_localstack_used + i] = ((int *)qc_globals)[f->parm_start + i];

	qc_localstack_used += c;

	/* copy parameters */
	o = f->parm_start;
	for (i = 0; i<f->numparms; i++)
	{
		for (j = 0; j < f->parm_size[i]; j++)
		{
			((int *)qc_globals)[o] = ((int *)qc_globals)[QC_OFS_PARM0 + i * 3 + j];
			o++;
		}
	}

	qc_xfunction = f;

	/* return starting statement */
	return f->first_statement - 1;
}

/* leave the current function */
int qc_function_leave(void)
{
	/* variables */
	int i, c;

	/* check for underflow */
	if (qc_depth <= 0)
		qc_error("qc_function_leave(): stack underflow");

	/* restore locals from the stack */
	c = qc_xfunction->locals;
	qc_localstack_used -= c;

	/* check for underflow */
	if (qc_localstack_used < 0)
		qc_error("qc_function_leave(): locals stack underflow");

	for (i = 0; i < c; i++)
		((int *)qc_globals)[qc_xfunction->parm_start + i] = qc_localstack[qc_localstack_used + i];

	/* up stack */
	qc_depth--;
	qc_xfunction = qc_stack[qc_depth].f;

	return qc_stack[qc_depth].s;
}


/*
====================
PR_ExecuteProgram
====================
*/
void qc_execute(int fnum)
{
	qc_eval_t *a, *b, *c;
	int s;
	qc_statement_t *st;
	qc_function_t *f, *newf;
	int runaway;
	int i;
	int exitdepth;
	qc_eval_t *ptr;

	if (!fnum || fnum >= qc->num_functions)
	{
		qc_error("qc_execute(): null functtion %d", fnum);
		return;
	}

	f = &qc_functions[fnum];

	/* check for runaway loop, and trace */
	runaway = 100000;
	qc_trace = QC_FALSE;

	/* make a stack frame */
	exitdepth = qc_depth;

	/* enter function */
	s = qc_function_enter(f);

	/* main execution loop */
	while (1)
	{
		/* next statement */
		s++;

		printf("statement: %d\n", s);

		/* get statement and eval */
		st = &qc_statements[s];
		a = (qc_eval_t *)&qc_globals[st->a];
		b = (qc_eval_t *)&qc_globals[st->b];
		c = (qc_eval_t *)&qc_globals[st->c];

		printf("st->a: %d\n", st->a);
		printf("st->b: %d\n", st->b);
		printf("st->c: %d\n", st->c);

		/* protect against runaway loop */
		if (!--runaway)
			qc_error("qc_execute(): runaway loop error");

		qc_xfunction->profile++;
		qc_xstatement = s;

		/*
		if (qc_trace)
			PR_PrintStatement(st);
		*/

		/* opcode processing */
		printf("opcode: %d\n", st->op);
		switch (st->op)
		{
			case QC_OP_ADD_F:
				c->_float = a->_float + b->_float;
				break;

			case QC_OP_ADD_V:
				c->vector[0] = a->vector[0] + b->vector[0];
				c->vector[1] = a->vector[1] + b->vector[1];
				c->vector[2] = a->vector[2] + b->vector[2];
				break;

			case QC_OP_SUB_F:
				c->_float = a->_float - b->_float;
				break;

			case QC_OP_SUB_V:
				c->vector[0] = a->vector[0] - b->vector[0];
				c->vector[1] = a->vector[1] - b->vector[1];
				c->vector[2] = a->vector[2] - b->vector[2];
				break;

			case QC_OP_MUL_F:
				c->_float = a->_float * b->_float;
				break;

			case QC_OP_MUL_V:
				c->_float = a->vector[0]*b->vector[0]
						+ a->vector[1]*b->vector[1]
						+ a->vector[2]*b->vector[2];
				break;

			case QC_OP_MUL_FV:
				c->vector[0] = a->_float * b->vector[0];
				c->vector[1] = a->_float * b->vector[1];
				c->vector[2] = a->_float * b->vector[2];
				break;

			case QC_OP_MUL_VF:
				c->vector[0] = b->_float * a->vector[0];
				c->vector[1] = b->_float * a->vector[1];
				c->vector[2] = b->_float * a->vector[2];
				break;

			case QC_OP_DIV_F:
				c->_float = a->_float / b->_float;
				break;

			case QC_OP_BITAND:
				c->_float = (int)a->_float & (int)b->_float;
				break;

			case QC_OP_BITOR:
				c->_float = (int)a->_float | (int)b->_float;
				break;

			case QC_OP_GE:
				c->_float = a->_float >= b->_float;
				break;

			case QC_OP_LE:
				c->_float = a->_float <= b->_float;
				break;

			case QC_OP_GT:
				c->_float = a->_float > b->_float;
				break;

			case QC_OP_LT:
				c->_float = a->_float < b->_float;
				break;

			case QC_OP_AND:
				c->_float = a->_float && b->_float;
				break;

			case QC_OP_OR:
				c->_float = a->_float || b->_float;
				break;

			case QC_OP_NOT_F:
				c->_float = !a->_float;
				break;

			case QC_OP_NOT_V:
				c->_float = !a->vector[0] && !a->vector[1] && !a->vector[2];
				break;

			case QC_OP_NOT_S:
				c->_float = !a->string || !qc_strings[a->string];
				break;

			case QC_OP_NOT_FNC:
				c->_float = !a->function;
				break;

			case QC_OP_NOT_ENT:
				/*
				c->_float = (PROG_TO_EDICT(a->edict) == sv.edicts);
				*/
				break;

			case QC_OP_EQ_F:
				c->_float = a->_float == b->_float;
				break;

			case QC_OP_EQ_V:
				c->_float = (a->vector[0] == b->vector[0]) &&
							(a->vector[1] == b->vector[1]) &&
							(a->vector[2] == b->vector[2]);
				break;

			case QC_OP_EQ_S:
				c->_float = !strcmp(qc_strings + a->string, qc_strings + b->string);
				break;

			case QC_OP_EQ_E:
				c->_float = a->_int == b->_int;
				break;

			case QC_OP_EQ_FNC:
				c->_float = a->function == b->function;
				break;

			case QC_OP_NE_F:
				c->_float = a->_float != b->_float;
				break;

			case QC_OP_NE_V:
				c->_float = (a->vector[0] != b->vector[0]) ||
							(a->vector[1] != b->vector[1]) ||
							(a->vector[2] != b->vector[2]);
				break;

			case QC_OP_NE_S:
				c->_float = strcmp(qc_strings + a->string, qc_strings + b->string);
				break;

			case QC_OP_NE_E:
				c->_float = a->_int != b->_int;
				break;

			case QC_OP_NE_FNC:
				c->_float = a->function != b->function;
				break;

			case QC_OP_STORE_F:
			case QC_OP_STORE_ENT:
			case QC_OP_STORE_FLD: /* integers */
			case QC_OP_STORE_S:
			case QC_OP_STORE_FNC: /* pointers */
				b->_int = a->_int;
				break;

			case QC_OP_STORE_V:
				b->vector[0] = a->vector[0];
				b->vector[1] = a->vector[1];
				b->vector[2] = a->vector[2];
				break;

			case QC_OP_STOREP_F:
			case QC_OP_STOREP_ENT:
			case QC_OP_STOREP_FLD: /* integers */
			case QC_OP_STOREP_S:
			case QC_OP_STOREP_FNC: /* pointers */
				/*
				ptr = (qc_eval_t *)((byte *)sv.edicts + b->_int);
				ptr->_int = a->_int;
				*/
				break;
			case QC_OP_STOREP_V:
				/*
				ptr = (qc_eval_t *)((byte *)sv.edicts + b->_int);
				ptr->vector[0] = a->vector[0];
				ptr->vector[1] = a->vector[1];
				ptr->vector[2] = a->vector[2];
				*/
				break;

			case QC_OP_ADDRESS:
				/*
				ed = PROG_TO_EDICT(a->edict);
				if (ed == (edict_t *)sv.edicts && sv.state == ss_active)
					PR_RunError ("assignment to world entity");
				c->_int = (byte *)((int *)&ed->v + b->_int) - (byte *)sv.edicts;
				*/
				break;

			case QC_OP_LOAD_F:
			case QC_OP_LOAD_FLD:
			case QC_OP_LOAD_ENT:
			case QC_OP_LOAD_S:
			case QC_OP_LOAD_FNC:
				/*
				ed = PROG_TO_EDICT(a->edict);
				a = (qc_eval_t *)((int *)&ed->v + b->_int);
				c->_int = a->_int;
				*/
				break;

			case QC_OP_LOAD_V:
				/*
				ed = PROG_TO_EDICT(a->edict);
				a = (qc_eval_t *)((int *)&ed->v + b->_int);
				c->vector[0] = a->vector[0];
				c->vector[1] = a->vector[1];
				c->vector[2] = a->vector[2];
				*/
				break;

			case QC_OP_IFNOT:
				if (!a->_int)
					s += st->b - 1; /* offset the s++ */
				break;

			case QC_OP_IF:
				if (a->_int)
					s += st->b - 1;	/* offset the s++ */
				break;

			case QC_OP_GOTO:
				s += st->a - 1;	/* offset the s++ */
				break;

			case QC_OP_CALL0:
			case QC_OP_CALL1:
			case QC_OP_CALL2:
			case QC_OP_CALL3:
			case QC_OP_CALL4:
			case QC_OP_CALL5:
			case QC_OP_CALL6:
			case QC_OP_CALL7:
			case QC_OP_CALL8:
				qc_argc = st->op - QC_OP_CALL0;
				if (!a->function)
					qc_error("qc_execute(): null function %d", a->function);

				newf = &qc_functions[a->function];

				/* negative statements are built in functions */
				if (newf->first_statement < 0)
				{
					i = -newf->first_statement;

					if (i >= qcvm_num_builtins)
						qc_error("qc_execute(): bad builtin %d", i);;

					qc_builtins[i] ();
					break;
				}

				s = qc_function_enter(newf);
				break;

			case QC_OP_DONE:
			case QC_OP_RETURN:
				qc_globals[QC_OFS_RETURN] = qc_globals[st->a];
				qc_globals[QC_OFS_RETURN + 1] = qc_globals[st->a + 1];
				qc_globals[QC_OFS_RETURN + 2] = qc_globals[st->a + 2];

				s = qc_function_leave();

				if (qc_depth == exitdepth)
					return; 

				break;

			case QC_OP_STATE:
				/*
				ed = PROG_TO_EDICT(qc_global_struct->self);
				ed->v.nextthink = qc_global_struct->time + 0.1;

				if (a->_float != ed->v.frame)
				{
					ed->v.frame = a->_float;
				}

				ed->v.think = b->function;
				*/
				break;

			default:
				qc_error("qc_execute(): bad opcode %d", st->op);;
		}
	}
}

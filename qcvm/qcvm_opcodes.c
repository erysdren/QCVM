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
#include <string.h>
#include <stdarg.h>

/* qcvm */
#include "qcvm_private.h"

/*
 * opcode function macro
 */

#define QCVM_OPCODE_FUNC(o) void qcvm_op_##o(qcvm_t *qcvm)
#define QCVM_CALL_OPCODE_FUNC(o) qcvm_op_##o(qcvm)

/*
 * helpers
 */

QCVM_OPCODE_FUNC(_NOT_IMPLEMENTED)
{
	fprintf(stderr, "error: opcode %s is not implemented\n", qcvm_opcode_table[qcvm->statement_p->opcode].name);
	qcvm->fail = 1;
}

QCVM_OPCODE_FUNC(_DONE_RETURN)
{
	qcvm->globals[OFS_RETURN] = qcvm->globals[qcvm->statement_p->vars[0]];
	qcvm->globals[OFS_RETURN + 1] = qcvm->globals[qcvm->statement_p->vars[0] + 1];
	qcvm->globals[OFS_RETURN + 2] = qcvm->globals[qcvm->statement_p->vars[0] + 2];

	qcvm->statement_i = qcvm_function_close(qcvm);

	if (qcvm->stack_depth == qcvm->exit_depth)
		qcvm->done = 1;
}

QCVM_OPCODE_FUNC(_CALL)
{
	/* get argument count of next function */
	if (qcvm->statement_p->opcode > OPCODE_CALL8)
		qcvm->function_argc = qcvm->statement_p->opcode - (OPCODE_CALL1H - 1);
	else
		qcvm->function_argc = qcvm->statement_p->opcode - OPCODE_CALL0;

	/* check for null function */
	if (!qcvm->eval_p[1]->function)
	{
		fprintf(stderr, "error: null function\n");
		qcvm->fail = 1;
		return;
	}

	/* assign next function value */
	qcvm->nextfunction_p = &qcvm->functions[qcvm->eval_p[1]->function];

	/* check if this is a named builtin */
	if (qcvm->nextfunction_p->first_statement == 0)
	{
		qcvm->export_i = qcvm_find_export(qcvm, GET_STRING_OFS(qcvm->nextfunction_p->name));

		if (qcvm->export_i < 0)
		{
			fprintf(stderr, "error: null function\n");
			qcvm->fail = 1;
			return;
		}

		qcvm->exports[qcvm->export_i].func(qcvm);
		qcvm->nextfunction_p->first_statement = -qcvm->export_i;

		return;
	}

	/* negative statements are functions exported from c */
	if (qcvm->nextfunction_p->first_statement < 0)
	{
		qcvm->export_i = -qcvm->nextfunction_p->first_statement;

		if (!qcvm->export_i || !qcvm->exports)
		{
			fprintf(stderr, "error: null function\n");
			qcvm->fail = 1;
			return;
		}

		qcvm->exports[qcvm->export_i].func(qcvm);

		return;
	}

	qcvm->statement_i = qcvm_function_setup(qcvm, qcvm->nextfunction_p);
}

QCVM_OPCODE_FUNC(_CALLH)
{
	if (qcvm->statement_p->opcode > OPCODE_CALL1H)
	{
		GET_VECTOR(OFS_PARM1)[0] = qcvm->eval_p[3]->vector[0];
		GET_VECTOR(OFS_PARM1)[1] = qcvm->eval_p[3]->vector[1];
		GET_VECTOR(OFS_PARM1)[2] = qcvm->eval_p[3]->vector[2];
	}

	GET_VECTOR(OFS_PARM0)[0] = qcvm->eval_p[2]->vector[0];
	GET_VECTOR(OFS_PARM0)[1] = qcvm->eval_p[2]->vector[1];
	GET_VECTOR(OFS_PARM0)[2] = qcvm->eval_p[2]->vector[2];

	QCVM_CALL_OPCODE_FUNC(_CALL);
}

/*
 * opcode function definitions
 */

QCVM_OPCODE_FUNC(DONE)
{
	QCVM_CALL_OPCODE_FUNC(_DONE_RETURN);
}

QCVM_OPCODE_FUNC(MUL_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ * qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(MUL_V)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->vector[0] * qcvm->eval_p[2]->vector[0] +
		qcvm->eval_p[1]->vector[1] * qcvm->eval_p[2]->vector[1] +
		qcvm->eval_p[1]->vector[2] * qcvm->eval_p[2]->vector[2];
}

QCVM_OPCODE_FUNC(MUL_FV)
{
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[1]->float_ * qcvm->eval_p[2]->vector[0];
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[1]->float_ * qcvm->eval_p[2]->vector[1];
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[1]->float_ * qcvm->eval_p[2]->vector[2];
}

QCVM_OPCODE_FUNC(MUL_VF)
{
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[2]->float_ * qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[2]->float_ * qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[2]->float_ * qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(DIV_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ / qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(ADD_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ + qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(ADD_V)
{
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[1]->vector[0] + qcvm->eval_p[2]->vector[0];
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[1]->vector[1] + qcvm->eval_p[2]->vector[1];
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[1]->vector[2] + qcvm->eval_p[2]->vector[2];
}

QCVM_OPCODE_FUNC(SUB_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ - qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(SUB_V)
{
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[1]->vector[0] - qcvm->eval_p[2]->vector[0];
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[1]->vector[1] - qcvm->eval_p[2]->vector[1];
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[1]->vector[2] - qcvm->eval_p[2]->vector[2];
}

QCVM_OPCODE_FUNC(EQ_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ == qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(EQ_V)
{
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[1]->vector[0] == qcvm->eval_p[2]->vector[0]) &&
		(qcvm->eval_p[1]->vector[1] == qcvm->eval_p[2]->vector[1]) &&
		(qcvm->eval_p[1]->vector[2] == qcvm->eval_p[2]->vector[2]);
}

QCVM_OPCODE_FUNC(EQ_S)
{
	qcvm->eval_p[3]->float_ = !strcmp(GET_STRING_OFS(qcvm->eval_p[1]->string), GET_STRING_OFS(qcvm->eval_p[2]->string));
}

QCVM_OPCODE_FUNC(EQ_E)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->int_ == qcvm->eval_p[2]->int_;
}

QCVM_OPCODE_FUNC(EQ_FNC)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->function == qcvm->eval_p[2]->function;
}

QCVM_OPCODE_FUNC(NE_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ != qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(NE_V)
{
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[1]->vector[0] != qcvm->eval_p[2]->vector[0]) ||
		(qcvm->eval_p[1]->vector[1] != qcvm->eval_p[2]->vector[1]) ||
		(qcvm->eval_p[1]->vector[2] != qcvm->eval_p[2]->vector[2]);
}

QCVM_OPCODE_FUNC(NE_S)
{
	qcvm->eval_p[3]->float_ = strcmp(GET_STRING_OFS(qcvm->eval_p[1]->string), GET_STRING_OFS(qcvm->eval_p[2]->string));
}

QCVM_OPCODE_FUNC(NE_E)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->int_ != qcvm->eval_p[2]->int_;
}

QCVM_OPCODE_FUNC(NE_FNC)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->function != qcvm->eval_p[2]->function;
}

QCVM_OPCODE_FUNC(LE)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ <= qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(GE)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ >= qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(LT)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ < qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(GT)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ > qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(LOAD_F)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[1] = (qcvm_evaluator_t *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(LOAD_V)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[1] = (qcvm_evaluator_t *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(LOAD_S)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[1] = (qcvm_evaluator_t *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(LOAD_ENT)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[1] = (qcvm_evaluator_t *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(LOAD_FLD)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[1] = (qcvm_evaluator_t *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(LOAD_FNC)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[1] = (qcvm_evaluator_t *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(ADDRESS)
{
	qcvm->entity_p = QC_TO_ENTITY(qcvm->eval_p[1]->entity);
	qcvm->eval_p[3]->int_ = (unsigned char *)((int *)&qcvm->entity_p->v + qcvm->eval_p[2]->int_) - (unsigned char *)qcvm->entities;
}

QCVM_OPCODE_FUNC(STORE_F)
{
	qcvm->eval_p[2]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STORE_V)
{
	qcvm->eval_p[2]->vector[0] = qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[2]->vector[1] = qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[2]->vector[2] = qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(STORE_S)
{
	qcvm->eval_p[2]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STORE_ENT)
{
	qcvm->eval_p[2]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STORE_FLD)
{
	qcvm->eval_p[2]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STORE_FNC)
{
	qcvm->eval_p[2]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STOREP_F)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[0]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STOREP_V)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[0]->vector[0] = qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[0]->vector[1] = qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[0]->vector[2] = qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(STOREP_S)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[0]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STOREP_ENT)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[0]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STOREP_FLD)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[0]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STOREP_FNC)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[0]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(RETURN)
{
	QCVM_CALL_OPCODE_FUNC(_DONE_RETURN);
}

QCVM_OPCODE_FUNC(NOT_F)
{
	qcvm->eval_p[3]->float_ = !qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(NOT_V)
{
	qcvm->eval_p[3]->float_ = !qcvm->eval_p[1]->vector[0] && !qcvm->eval_p[1]->vector[1] && !qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(NOT_S)
{
	qcvm->eval_p[3]->float_ = !qcvm->eval_p[1]->string || !qcvm->strings[qcvm->eval_p[1]->string];
}

QCVM_OPCODE_FUNC(NOT_ENT)
{
	qcvm->eval_p[3]->float_ = (QC_TO_ENTITY(qcvm->eval_p[1]->entity) == qcvm->entities);
}

QCVM_OPCODE_FUNC(NOT_FNC)
{
	qcvm->eval_p[3]->float_ = !qcvm->eval_p[1]->function;
}

QCVM_OPCODE_FUNC(IF)
{
	if (qcvm->eval_p[1]->int_) qcvm->statement_i += qcvm->statement_p->vars[1] - 1;
}

QCVM_OPCODE_FUNC(IFNOT)
{
	if (!qcvm->eval_p[1]->int_) qcvm->statement_i += qcvm->statement_p->vars[1] - 1;
}

QCVM_OPCODE_FUNC(CALL0)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL1)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL2)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL3)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL4)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL5)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL6)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL7)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(CALL8)
{
	QCVM_CALL_OPCODE_FUNC(_CALL);
}

QCVM_OPCODE_FUNC(STATE)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GOTO)
{
	qcvm->statement_i += qcvm->statement_p->vars[0] - 1;
}

QCVM_OPCODE_FUNC(AND_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ && qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(OR_F)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ || qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(BITAND_F)
{
	qcvm->eval_p[3]->float_ = (int)qcvm->eval_p[1]->float_ & (int)qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(BITOR_F)
{
	qcvm->eval_p[3]->float_ = (int)qcvm->eval_p[1]->float_ | (int)qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(MULSTORE_F)
{
	qcvm->eval_p[2]->float_ *= qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(MULSTORE_V)
{
	qcvm->eval_p[2]->vector[0] *= qcvm->eval_p[1]->float_;
	qcvm->eval_p[2]->vector[1] *= qcvm->eval_p[1]->float_;
	qcvm->eval_p[2]->vector[2] *= qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(MULSTOREP_F)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[0]->float_ *= qcvm->eval_p[1]->float_);
}

QCVM_OPCODE_FUNC(MULSTOREP_V)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->vector[0] = (qcvm->eval_p[0]->vector[0] *= qcvm->eval_p[1]->float_);
	qcvm->eval_p[3]->vector[0] = (qcvm->eval_p[0]->vector[1] *= qcvm->eval_p[1]->float_);
	qcvm->eval_p[3]->vector[0] = (qcvm->eval_p[0]->vector[2] *= qcvm->eval_p[1]->float_);
}

QCVM_OPCODE_FUNC(DIVSTORE_F)
{
	qcvm->eval_p[2]->float_ /= qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(DIVSTOREP_F)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[0]->float_ /= qcvm->eval_p[1]->float_);
}

QCVM_OPCODE_FUNC(ADDSTORE_F)
{
	qcvm->eval_p[2]->float_ += qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(ADDSTORE_V)
{
	qcvm->eval_p[2]->vector[0] += qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[2]->vector[1] += qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[2]->vector[2] += qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(ADDSTOREP_F)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[0]->float_ += qcvm->eval_p[1]->float_);
}

QCVM_OPCODE_FUNC(ADDSTOREP_V)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->vector[0] = (qcvm->eval_p[0]->vector[0] += qcvm->eval_p[1]->vector[0]);
	qcvm->eval_p[3]->vector[1] = (qcvm->eval_p[0]->vector[1] += qcvm->eval_p[1]->vector[1]);
	qcvm->eval_p[3]->vector[2] = (qcvm->eval_p[0]->vector[2] += qcvm->eval_p[1]->vector[2]);
}

QCVM_OPCODE_FUNC(SUBSTORE_F)
{
	qcvm->eval_p[2]->float_ -= qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(SUBSTORE_V)
{
	qcvm->eval_p[2]->vector[0] -= qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[2]->vector[1] -= qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[2]->vector[2] -= qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(SUBSTOREP_F)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[0]->float_ -= qcvm->eval_p[1]->float_);
}

QCVM_OPCODE_FUNC(SUBSTOREP_V)
{
	qcvm->eval_p[0] = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + qcvm->eval_p[2]->int_);
	qcvm->eval_p[3]->vector[0] = (qcvm->eval_p[0]->vector[0] -= qcvm->eval_p[1]->vector[0]);
	qcvm->eval_p[3]->vector[1] = (qcvm->eval_p[0]->vector[1] -= qcvm->eval_p[1]->vector[1]);
	qcvm->eval_p[3]->vector[2] = (qcvm->eval_p[0]->vector[2] -= qcvm->eval_p[1]->vector[2]);
}

QCVM_OPCODE_FUNC(FETCH_GBL_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(FETCH_GBL_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(FETCH_GBL_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(FETCH_GBL_E)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(FETCH_GBL_FNC)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CSTATE)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CWSTATE)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(THINKTIME)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITSET)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITSETP)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITCLR)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITCLRP)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RAND0)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RAND1)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RAND2)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RANDV0)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RANDV1)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RANDV2)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SWITCH_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SWITCH_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SWITCH_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SWITCH_E)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SWITCH_FNC)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CASE)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CASERANGE)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CALL1H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL2H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL3H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL4H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL5H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL6H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL7H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(CALL8H)
{
	QCVM_CALL_OPCODE_FUNC(_CALLH);
}

QCVM_OPCODE_FUNC(STORE_I)
{
	qcvm->eval_p[2]->int_ = qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STORE_IF)
{
	qcvm->eval_p[2]->float_ = (float)qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(STORE_FI)
{
	qcvm->eval_p[2]->int_ = (int)qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(ADD_I)
{
	qcvm->eval_p[3]->int_ = qcvm->eval_p[1]->int_ + qcvm->eval_p[2]->int_;
}

QCVM_OPCODE_FUNC(ADD_FI)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->float_ + (float)qcvm->eval_p[2]->int_;
}

QCVM_OPCODE_FUNC(ADD_IF)
{
	qcvm->eval_p[3]->float_ = (float)qcvm->eval_p[1]->int_ + qcvm->eval_p[2]->float_;
}

QCVM_OPCODE_FUNC(SUB_I)
{
	qcvm->eval_p[3]->float_ = qcvm->eval_p[1]->int_ - qcvm->eval_p[2]->int_;
}

QCVM_OPCODE_FUNC(SUB_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SUB_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_ITOF)
{
	qcvm->eval_p[3]->float_ = (float)qcvm->eval_p[1]->int_;
}

QCVM_OPCODE_FUNC(CONV_FTOI)
{
	qcvm->eval_p[3]->int_ = (int)qcvm->eval_p[1]->float_;
}

QCVM_OPCODE_FUNC(LOADP_ITOF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_FTOI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOAD_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREP_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREP_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREP_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITAND_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITOR_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(MUL_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(DIV_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(EQ_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(NE_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(IFNOT_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(IF_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(NOT_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(DIV_VF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITXOR_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RSHIFT_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LSHIFT_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOBALADDRESS)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(ADD_PIW)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_ENT)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_FLD)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_FNC)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STORE_P)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOAD_P)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_ENT)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_FLD)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_FNC)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GE_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GT_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GE_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GT_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GE_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GT_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(EQ_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(EQ_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(ADD_SF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SUB_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREP_C)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_C)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(MUL_IF)
{
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[1]->int_ * qcvm->eval_p[2]->float_);
}

QCVM_OPCODE_FUNC(MUL_FI)
{
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[1]->float_ * qcvm->eval_p[2]->int_);
}

QCVM_OPCODE_FUNC(MUL_VI)
{
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[1]->vector[0] * qcvm->eval_p[2]->int_;
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[1]->vector[1] * qcvm->eval_p[2]->int_;
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[1]->vector[2] * qcvm->eval_p[2]->int_;
}

QCVM_OPCODE_FUNC(MUL_IV)
{
	qcvm->eval_p[3]->vector[0] = qcvm->eval_p[2]->int_ * qcvm->eval_p[1]->vector[0];
	qcvm->eval_p[3]->vector[1] = qcvm->eval_p[2]->int_ * qcvm->eval_p[1]->vector[1];
	qcvm->eval_p[3]->vector[2] = qcvm->eval_p[2]->int_ * qcvm->eval_p[1]->vector[2];
}

QCVM_OPCODE_FUNC(DIV_IF)
{
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[1]->int_ / qcvm->eval_p[2]->float_);
}

QCVM_OPCODE_FUNC(DIV_FI)
{
	qcvm->eval_p[3]->float_ = (qcvm->eval_p[1]->float_ / qcvm->eval_p[2]->int_);
}

QCVM_OPCODE_FUNC(BITAND_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITOR_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITAND_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITOR_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(AND_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(OR_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(AND_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(OR_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(AND_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(OR_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(NE_IF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(NE_FI)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_ENT)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_FLD)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_FNC)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GSTOREP_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GADDRESS)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_FLD)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_ENT)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_FNC)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BOUNDCHECK)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(UNUSED)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(PUSH)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(POP)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SWITCH_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(GLOAD_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(IF_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(IFNOT_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREF_V)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREF_F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREF_S)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREF_I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREP_B)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_B)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_U)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_U)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(DIV_U)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RSHIFT_U)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(ADD_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SUB_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(MUL_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(DIV_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITAND_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITOR_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(BITXOR_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LSHIFT_I64I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RSHIFT_I64I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(EQ_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(NE_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_U64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_U64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(DIV_U64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(RSHIFT_U64I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STORE_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREP_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(STOREF_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOAD_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADA_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LOADP_I64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_UI64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_II64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_I64I)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_FD)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_DF)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_I64F)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_FI64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_I64D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(CONV_DI64)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(ADD_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(SUB_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(MUL_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(DIV_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LE_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(LT_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(EQ_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

QCVM_OPCODE_FUNC(NE_D)
{
	QCVM_CALL_OPCODE_FUNC(_NOT_IMPLEMENTED);
}

#undef QCVM_OPCODE_FUNC
#undef QCVM_CALL_OPCODE_FUNC

/*
 * opcode function table
 */

#define QCVM_OPCODE(o) [OPCODE_##o] = {.op = OPCODE_##o, .name = "OPCODE_" #o, .func = qcvm_op_##o},
qcvm_opcode_table_entry_t qcvm_opcode_table[] = {
	#include "qcvm_opcodes.h"
};
#undef QCVM_OPCODE

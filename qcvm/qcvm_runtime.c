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
	/* variables */
	qcvm_evaluator_t *eval1, *eval2, *eval3;
	qcvm_evaluator_t *eval_ptr;
	qcvm_statement_t *statement;
	qcvm_function_t *function, *nextfunction;
	qcvm_entity_t *entity;
	int exit_depth, st;
	int export;

	/* sanity check */
	if (!qcvm || func <= 0 || func >= qcvm->header->num_functions)
		return;

	/* assign working function */
	function = &qcvm->functions[func];
	exit_depth = qcvm->stack_depth;

	/* enter function */
	st = qcvm_function_setup(qcvm, function);

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
		st++;

		/* get current statement and evaluators */
		statement = &qcvm->statements[st];
		eval1 = (qcvm_evaluator_t *)&qcvm->globals[statement->vars[0]];
		eval2 = (qcvm_evaluator_t *)&qcvm->globals[statement->vars[1]];
		eval3 = (qcvm_evaluator_t *)&qcvm->globals[statement->vars[2]];

		/* update stack */
		qcvm->xstack.function->profile++;
		qcvm->xstack.statement = st;

		/* opcode processing tree */
		switch (statement->opcode)
		{
			/* add float and float */
			case OPCODE_ADD_F:
			{
				eval3->float_ = eval1->float_ + eval2->float_;
				break;
			}

			/* add vector and vector */
			case OPCODE_ADD_V:
			{
				eval3->vector[0] = eval1->vector[0] + eval2->vector[0];
				eval3->vector[1] = eval1->vector[1] + eval2->vector[1];
				eval3->vector[2] = eval1->vector[2] + eval2->vector[2];
				break;
			}

			/* subtract float and float */
			case OPCODE_SUB_F:
			{
				eval3->float_ = eval1->float_ - eval2->float_;
				break;
			}

			/* subtract vector and vector */
			case OPCODE_SUB_V:
			{
				eval3->vector[0] = eval1->vector[0] - eval2->vector[0];
				eval3->vector[1] = eval1->vector[1] - eval2->vector[1];
				eval3->vector[2] = eval1->vector[2] - eval2->vector[2];
				break;
			}

			/* multiply float by float */
			case OPCODE_MUL_F:
			{
				eval3->float_ = eval1->float_ * eval2->float_;
				break;
			}

			/* multiply vector by vector */
			case OPCODE_MUL_V:
			{
				eval3->float_ = eval1->vector[0] * eval2->vector[0] +
								eval1->vector[1] * eval2->vector[1] +
								eval1->vector[2] * eval2->vector[2];
				break;
			}

			/* multiply float by vector */
			case OPCODE_MUL_FV:
			{
				eval3->vector[0] = eval1->float_ * eval2->vector[0];
				eval3->vector[1] = eval1->float_ * eval2->vector[1];
				eval3->vector[2] = eval1->float_ * eval2->vector[2];
				break;
			}

			/* multiply float by vector */
			case OPCODE_MUL_VF:
			{
				eval3->vector[0] = eval2->float_ * eval1->vector[0];
				eval3->vector[1] = eval2->float_ * eval1->vector[1];
				eval3->vector[2] = eval2->float_ * eval1->vector[2];
				break;
			}

			/* divide float by float */
			case OPCODE_DIV_F:
			{
				eval3->float_ = eval1->float_ / eval2->float_;
				break;
			}

			/* bitwise and */
			case OPCODE_BITAND:
			{
				eval3->float_ = (int)eval1->float_ & (int)eval2->float_;
				break;
			}

			/* bitwise or */
			case OPCODE_BITOR:
			{
				eval3->float_ = (int)eval1->float_ | (int)eval2->float_;
				break;
			}

			/* greater than or equal to */
			case OPCODE_GE:
			{
				eval3->float_ = eval1->float_ >= eval2->float_;
				break;
			}

			/* less than or equal to */
			case OPCODE_LE:
			{
				eval3->float_ = eval1->float_ <= eval2->float_;
				break;
			}

			/* greater than */
			case OPCODE_GT:
			{
				eval3->float_ = eval1->float_ > eval2->float_;
				break;
			}

			/* less than */
			case OPCODE_LT:
			{
				eval3->float_ = eval1->float_ < eval2->float_;
				break;
			}

			/* and */
			case OPCODE_AND:
			{
				eval3->float_ = eval1->float_ && eval2->float_;
				break;
			}

			/* or */
			case OPCODE_OR:
			{
				eval3->float_ = eval1->float_ || eval2->float_;
				break;
			}

			/* not float */
			case OPCODE_NOT_F:
			{
				eval3->float_ = !eval1->float_;
				break;
			}

			/* not vector */
			case OPCODE_NOT_V:
			{
				eval3->float_ = !eval1->vector[0] && !eval1->vector[1] && !eval1->vector[2];
				break;
			}

			/* not string */
			case OPCODE_NOT_S:
			{
				eval3->float_ = !eval1->string || !qcvm->strings[eval1->string];
				break;
			}

			/* not function */
			case OPCODE_NOT_FNC:
			{
				eval3->float_ = !eval1->function;
				break;
			}

			/* not entity */
			case OPCODE_NOT_ENT:
			{
				eval3->float_ = (QC_TO_ENTITY(eval1->entity) == qcvm->entities);
				break;
			}

			/* check if a float is equal to another */
			case OPCODE_EQ_F:
			{
				eval3->float_ = eval1->float_ == eval2->float_;
				break;
			}

			/* check if a vector is equal to another */
			case OPCODE_EQ_V:
			{
				eval3->float_ = (eval1->vector[0] == eval2->vector[0]) &&
								(eval1->vector[1] == eval2->vector[1]) &&
								(eval1->vector[2] == eval2->vector[2]);
				break;
			}

			/* check if one string is equal to another */
			case OPCODE_EQ_S:
			{
				eval3->float_ = !strcmp(GET_STRING_OFS(eval1->string), GET_STRING_OFS(eval2->string));
				break;
			}

			/* check if one entity is equal to another */
			case OPCODE_EQ_E:
			{
				eval3->float_ = eval1->int_ == eval2->int_;
				break;
			}

			/* check if one function is equal to another */
			case OPCODE_EQ_FNC:
			{
				eval3->float_ = eval1->function == eval2->function;
				break;
			}

			/* check if a float is not equal to another */
			case OPCODE_NE_F:
			{
				eval3->float_ = eval1->float_ != eval2->float_;
				break;
			}

			/* check if a vector is not equal to another */
			case OPCODE_NE_V:
			{
				eval3->float_ = (eval1->vector[0] != eval2->vector[0]) ||
								(eval1->vector[1] != eval2->vector[1]) ||
								(eval1->vector[2] != eval2->vector[2]);
				break;
			}

			/* check if one string is equal to another */
			case OPCODE_NE_S:
			{
				eval3->float_ = strcmp(GET_STRING_OFS(eval1->string), GET_STRING_OFS(eval2->string));
				break;
			}

			/* check if one entity is not equal to another */
			case OPCODE_NE_E:
			{
				eval3->float_ = eval1->int_ != eval2->int_;
				break;
			}

			/* check if one function is equal to another */
			case OPCODE_NE_FNC:
			{
				eval3->float_ = eval1->function != eval2->function;
				break;
			}

			/* store a 4-byte value in another */
			case OPCODE_STORE_F:
			case OPCODE_STORE_ENT:
			case OPCODE_STORE_FLD:
			case OPCODE_STORE_S:
			case OPCODE_STORE_FNC:
			{
				eval2->int_ = eval1->int_;
				break;
			}

			/* store a 12-byte value (vector) in another */
			case OPCODE_STORE_V:
			{
				eval2->vector[0] = eval1->vector[0];
				eval2->vector[1] = eval1->vector[1];
				eval2->vector[2] = eval1->vector[2];
				break;
			}

			/* store a 4-byte value in an entity field */
			case OPCODE_STOREP_F:
			case OPCODE_STOREP_ENT:
			case OPCODE_STOREP_FLD:
			case OPCODE_STOREP_S:
			case OPCODE_STOREP_FNC:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval_ptr->int_ = eval1->int_;
				break;
			}

			/* store a 12-byte value (vector) in an entity field */
			case OPCODE_STOREP_V:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval_ptr->vector[0] = eval1->vector[0];
				eval_ptr->vector[1] = eval1->vector[1];
				eval_ptr->vector[2] = eval1->vector[2];
				break;
			}

			case OPCODE_MULSTORE_F:
			{
				eval2->float_ *= eval1->float_;
				break;
			}

			case OPCODE_MULSTORE_V:
			{
				eval2->vector[0] *= eval1->float_;
				eval2->vector[1] *= eval1->float_;
				eval2->vector[2] *= eval1->float_;
				break;
			}

			case OPCODE_MULSTOREP_F:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->float_ = (eval_ptr->float_ *= eval1->float_);
				break;
			}

			case OPCODE_MULSTOREP_V:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->vector[0] = (eval_ptr->vector[0] *= eval1->float_);
				eval3->vector[0] = (eval_ptr->vector[1] *= eval1->float_);
				eval3->vector[0] = (eval_ptr->vector[2] *= eval1->float_);
				break;
			}

			case OPCODE_DIVSTORE_F:
			{
				eval2->float_ /= eval1->float_;
				break;
			}

			case OPCODE_DIVSTOREP_F:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->float_ = (eval_ptr->float_ /= eval1->float_);
				break;
			}

			case OPCODE_ADDSTORE_F:
			{
				eval2->float_ += eval1->float_;
				break;
			}

			case OPCODE_ADDSTORE_V:
			{
				eval2->vector[0] += eval1->vector[0];
				eval2->vector[1] += eval1->vector[1];
				eval2->vector[2] += eval1->vector[2];
				break;
			}

			case OPCODE_ADDSTOREP_F:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->float_ = (eval_ptr->float_ += eval1->float_);
				break;
			}

			case OPCODE_ADDSTOREP_V:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->vector[0] = (eval_ptr->vector[0] += eval1->vector[0]);
				eval3->vector[1] = (eval_ptr->vector[1] += eval1->vector[1]);
				eval3->vector[2] = (eval_ptr->vector[2] += eval1->vector[2]);
				break;
			}

			case OPCODE_SUBSTORE_F:
			{
				eval2->float_ -= eval1->float_;
				break;
			}

			case OPCODE_SUBSTORE_V:
			{
				eval2->vector[0] -= eval1->vector[0];
				eval2->vector[1] -= eval1->vector[1];
				eval2->vector[2] -= eval1->vector[2];
				break;
			}

			case OPCODE_SUBSTOREP_F:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->float_ = (eval_ptr->float_ -= eval1->float_);
				break;
			}

			case OPCODE_SUBSTOREP_V:
			{
				eval_ptr = (qcvm_evaluator_t *)((unsigned char *)qcvm->entities + eval2->int_);
				eval3->vector[0] = (eval_ptr->vector[0] -= eval1->vector[0]);
				eval3->vector[1] = (eval_ptr->vector[1] -= eval1->vector[1]);
				eval3->vector[2] = (eval_ptr->vector[2] -= eval1->vector[2]);
				break;
			}

			/* get the address of an entity */
			case OPCODE_ADDRESS:
			{
				entity = QC_TO_ENTITY(eval1->entity);
				eval3->int_ = (unsigned char *)((int *)&entity->v + eval2->int_) - (unsigned char *)qcvm->entities;
				break;
			}

			/* load a 4-byte value from an entity field */
			case OPCODE_LOAD_F:
			case OPCODE_LOAD_FLD:
			case OPCODE_LOAD_ENT:
			case OPCODE_LOAD_S:
			case OPCODE_LOAD_FNC:
			{
				entity = QC_TO_ENTITY(eval1->entity);
				eval1 = (qcvm_evaluator_t *)((int *)&entity->v + eval2->int_);
				eval3->int_ = eval1->int_;
				break;
			}

			/* load a 12-byte value (vector) from an entity field */
			case OPCODE_LOAD_V:
			{
				entity = QC_TO_ENTITY(eval1->entity);
				eval1 = (qcvm_evaluator_t *)((int *)&entity->v + eval2->int_);
				eval3->vector[0] = eval1->vector[0];
				eval3->vector[1] = eval1->vector[1];
				eval3->vector[2] = eval1->vector[2];
				break;
			}

			/* if not */
			case OPCODE_IFNOT:
			{
				if (!eval1->int_) st += statement->vars[1] - 1;
				break;
			}

			/* if */
			case OPCODE_IF:
			{
				if (eval1->int_) st += statement->vars[1] - 1;
				break;
			}

			/* goto */
			case OPCODE_GOTO:
			{
				st += statement->vars[0] - 1;
				break;
			}

			case OPCODE_CALL0:
			case OPCODE_CALL1:
			case OPCODE_CALL2:
			case OPCODE_CALL3:
			case OPCODE_CALL4:
			case OPCODE_CALL5:
			case OPCODE_CALL6:
			case OPCODE_CALL7:
			case OPCODE_CALL8:
			{
				/* get argument count of next function */
				qcvm->function_argc = statement->opcode - OPCODE_CALL0;

				/* check for null function */
				if (!eval1->function)
				{
					fprintf(stderr, "error: null function\n");
					break;
				}

				/* assign next function value */
				nextfunction = &qcvm->functions[eval1->function];

				/* check if this is a named builtin */
				if (nextfunction->first_statement == 0)
				{
					export = qcvm_find_export(qcvm, GET_STRING_OFS(nextfunction->name));
					if (export < 0) break;
					qcvm->exports[export].func(qcvm);
					nextfunction->first_statement = -export;
					break;
				}

				/* negative statements are functions exported from c */
				if (nextfunction->first_statement < 0)
				{
					export = -nextfunction->first_statement;

					if (!export || !qcvm->exports)
					{
						fprintf(stderr, "error: null function\n");
						break;
					}

					qcvm->exports[export].func(qcvm);
					break;
				}

				st = qcvm_function_setup(qcvm, nextfunction);

				break;
			}

			/* return whatever value is required, and close the function */
			case OPCODE_DONE:
			case OPCODE_RETURN:
			{
				qcvm->globals[OFS_RETURN] = qcvm->globals[statement->vars[0]];
				qcvm->globals[OFS_RETURN + 1] = qcvm->globals[statement->vars[0] + 1];
				qcvm->globals[OFS_RETURN + 2] = qcvm->globals[statement->vars[0] + 2];

				st = qcvm_function_close(qcvm);

				if (qcvm->stack_depth == exit_depth)
					return;

				break;
			}

			/* update entity state */
			case OPCODE_STATE:
			{
				fprintf(stderr, "error: OPCODE_STATE not yet implemented\n");
				break;
			}

			/* unknown opcode */
			default:
			{
				fprintf(stderr, "error: unknown opcode\n");
				break;
			}
		}
	}
}

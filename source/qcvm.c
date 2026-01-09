/*
MIT License

Copyright (c) 2023-2026 erysdren (it/its)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <qcvm/qcvm.h>

/* for strcmp and strlen */
#if QCVM_NO_STDLIB
#ifdef QCVM_STRLEN
extern size_t QCVM_STRLEN(const char *s);
#else
static size_t QCVM_STRLEN(const char *s)
{
#if defined(__has_builtin) && __has_builtin(__builtin_strlen)
	return __builtin_strlen(s);
#else
	size_t len;
	for (len = 0; s[len]; ++len) ;
	return len;
#endif
}
#endif
#ifdef QCVM_STRCMP
extern int QCVM_STRCMP(const char *a, const char *b);
#else
static int QCVM_STRCMP(const char *a, const char *b)
{
#if defined(__has_builtin) && __has_builtin(__builtin_strcmp)
	return __builtin_strcmp(a, b);
#else
	while (*a && (*a == *b)) a++, b++;
	return *(const unsigned char *)a - *(const unsigned char *)b;
#endif
}
#endif
#else
#include <string.h>
#define QCVM_STRLEN(s) strlen(s)
#define QCVM_STRCMP(a, b) strcmp(a, b)
#endif

/* endian handling */
#if QCVM_BIG_ENDIAN
static uint16_t swap16(uint16_t n)
{
	return
		((n >> 8) & 0x00FF) |
		((n << 8) & 0xFF00);
}

static uint32_t swap32(uint32_t n)
{
	return
		((n >> 24) & 0x000000FF) |
		((n >> 8) & 0x0000FF00) |
		((n << 8) & 0x00FF0000) |
		((n << 24) & 0xFF000000);
}

static float swapfloat(float f)
{
	uint32_t n = swap32(*(uint32_t *)&f);
	return *(float *)&n;
}
#define LITTLE16(n) swap16(n)
#define LITTLE32(n) swap32(n)
#define LITTLEFLOAT(n) swapfloat(n)
#else
#define LITTLE16(n) (n)
#define LITTLE32(n) (n)
#define LITTLEFLOAT(n) (n)
#endif

/* recognized version values */
static const uint32_t progs_version_old = 3;
static const uint32_t progs_version_standard = 6;
static const uint32_t progs_version_extended = 7;

/* static offsets into globals table */
#define OFS_NULL (0)
#define OFS_RETURN (1)
#define OFS_PARM0 (4)
#define OFS_PARM1 (7)
#define OFS_PARM2 (10)
#define OFS_PARM3 (13)
#define OFS_PARM4 (16)
#define OFS_PARM5 (19)
#define OFS_PARM6 (22)
#define OFS_PARM7 (25)
#define OFS_RESERVED (28)

#define FIELD_PTR(e, o) (&((uint32_t *)qcvm->entities + ((e) * qcvm->header.num_entity_fields))[(o)])

/* opcodes */
enum {
	/* vanilla */
	OPCODE_DONE, OPCODE_MUL_F, OPCODE_MUL_V, OPCODE_MUL_FV, OPCODE_MUL_VF,
	OPCODE_DIV_F, OPCODE_ADD_F, OPCODE_ADD_V, OPCODE_SUB_F, OPCODE_SUB_V,
	OPCODE_EQ_F, OPCODE_EQ_V, OPCODE_EQ_S, OPCODE_EQ_E, OPCODE_EQ_FNC,
	OPCODE_NE_F, OPCODE_NE_V, OPCODE_NE_S, OPCODE_NE_E, OPCODE_NE_FNC,
	OPCODE_LE, OPCODE_GE, OPCODE_LT, OPCODE_GT, OPCODE_LOAD_F, OPCODE_LOAD_V,
	OPCODE_LOAD_S, OPCODE_LOAD_ENT, OPCODE_LOAD_FLD, OPCODE_LOAD_FNC,
	OPCODE_ADDRESS, OPCODE_STORE_F, OPCODE_STORE_V, OPCODE_STORE_S,
	OPCODE_STORE_ENT, OPCODE_STORE_FLD, OPCODE_STORE_FNC, OPCODE_STOREP_F,
	OPCODE_STOREP_V, OPCODE_STOREP_S, OPCODE_STOREP_ENT, OPCODE_STOREP_FLD,
	OPCODE_STOREP_FNC, OPCODE_RETURN, OPCODE_NOT_F, OPCODE_NOT_V, OPCODE_NOT_S,
	OPCODE_NOT_ENT, OPCODE_NOT_FNC, OPCODE_IF, OPCODE_IFNOT, OPCODE_CALL0,
	OPCODE_CALL1, OPCODE_CALL2, OPCODE_CALL3, OPCODE_CALL4, OPCODE_CALL5,
	OPCODE_CALL6, OPCODE_CALL7, OPCODE_CALL8, OPCODE_STATE, OPCODE_GOTO,
	OPCODE_AND_F, OPCODE_OR_F, OPCODE_BITAND_F, OPCODE_BITOR_F,

	NUM_OPCODES
};

int qcvm_init(qcvm_t *qcvm)
{
	struct qcvm_header *header;
	size_t i;

	if (!qcvm)
		return QCVM_NULL_POINTER;
	if (!qcvm->progs || !qcvm->len_progs)
		return QCVM_INVALID_PROGS;

	/* file header */
	header = (struct qcvm_header *)qcvm->progs;

	/* fixup endianness */
	qcvm->header.version = LITTLE32(header->version);
	qcvm->header.crc = LITTLE32(header->crc);
	qcvm->header.ofs_statements = LITTLE32(header->ofs_statements);
	qcvm->header.num_statements = LITTLE32(header->num_statements);
	qcvm->header.ofs_global_vars = LITTLE32(header->ofs_global_vars);
	qcvm->header.num_global_vars = LITTLE32(header->num_global_vars);
	qcvm->header.ofs_field_vars = LITTLE32(header->ofs_field_vars);
	qcvm->header.num_field_vars = LITTLE32(header->num_field_vars);
	qcvm->header.ofs_functions = LITTLE32(header->ofs_functions);
	qcvm->header.num_functions = LITTLE32(header->num_functions);
	qcvm->header.ofs_strings = LITTLE32(header->ofs_strings);
	qcvm->header.len_strings = LITTLE32(header->len_strings);
	qcvm->header.ofs_globals = LITTLE32(header->ofs_globals);
	qcvm->header.num_globals = LITTLE32(header->num_globals);
	qcvm->header.num_entity_fields = LITTLE32(header->num_entity_fields);

	/* check recognized versions */
	if (qcvm->header.version == progs_version_old)
		return QCVM_UNSUPPORTED_VERSION;
	else if (qcvm->header.version == progs_version_extended)
		return QCVM_UNSUPPORTED_VERSION;
	else if (qcvm->header.version != progs_version_standard)
		return QCVM_INVALID_PROGS;

	/* other sanity checks */
	if (!qcvm->tempstrings)
		return QCVM_NO_TEMPSTRINGS;
	if (!qcvm->entities)
		return QCVM_NO_ENTITIES;

	/* tempstrings */
	qcvm->tempstrings[0] = '\0';
	qcvm->tempstrings_ptr = qcvm->tempstrings + 1;

	/* statements */
	qcvm->num_statements = qcvm->header.num_statements;
	qcvm->statements = (struct qcvm_statement *)((uint8_t *)qcvm->progs + qcvm->header.ofs_statements);

	/* fixup endianness */
	for (i = 0; i < qcvm->num_statements; i++)
	{
		qcvm->statements[i].opcode = LITTLE16(qcvm->statements[i].opcode);
		qcvm->statements[i].vars[0] = LITTLE16(qcvm->statements[i].vars[0]);
		qcvm->statements[i].vars[1] = LITTLE16(qcvm->statements[i].vars[1]);
		qcvm->statements[i].vars[2] = LITTLE16(qcvm->statements[i].vars[2]);
	}

	/* functions */
	qcvm->num_functions = qcvm->header.num_functions;
	qcvm->functions = (struct qcvm_function *)((uint8_t *)qcvm->progs + qcvm->header.ofs_functions);

	/* fixup endianness */
	for (i = 0; i < qcvm->num_functions; i++)
	{
		qcvm->functions[i].first_statement = LITTLE32(qcvm->functions[i].first_statement);
		qcvm->functions[i].first_parm = LITTLE32(qcvm->functions[i].first_parm);
		qcvm->functions[i].num_locals = LITTLE32(qcvm->functions[i].num_locals);
		qcvm->functions[i].profile = LITTLE32(qcvm->functions[i].profile);
		qcvm->functions[i].ofs_name = LITTLE32(qcvm->functions[i].ofs_name);
		qcvm->functions[i].ofs_filename = LITTLE32(qcvm->functions[i].ofs_filename);
		qcvm->functions[i].num_parms = LITTLE32(qcvm->functions[i].num_parms);
	}

	/* strings */
	qcvm->len_strings = qcvm->header.len_strings;
	qcvm->strings = (char *)((uint8_t *)qcvm->progs + qcvm->header.ofs_strings);

	/* field vars */
	qcvm->num_field_vars = qcvm->header.num_field_vars;
	qcvm->field_vars = (struct qcvm_var *)((uint8_t *)qcvm->progs + qcvm->header.ofs_field_vars);

	/* fixup endianness */
	for (i = 0; i < qcvm->num_field_vars; i++)
	{
		qcvm->field_vars[i].type = LITTLE16(qcvm->field_vars[i].type);
		qcvm->field_vars[i].ofs = LITTLE16(qcvm->field_vars[i].ofs);
		qcvm->field_vars[i].name = LITTLE32(qcvm->field_vars[i].name);
	}

	/* global vars */
	qcvm->num_global_vars = qcvm->header.num_global_vars;
	qcvm->global_vars = (struct qcvm_var *)((uint8_t *)qcvm->progs + qcvm->header.ofs_global_vars);

	/* fixup endianness */
	for (i = 0; i < qcvm->num_global_vars; i++)
	{
		qcvm->global_vars[i].type = LITTLE16(qcvm->global_vars[i].type);
		qcvm->global_vars[i].ofs = LITTLE16(qcvm->global_vars[i].ofs);
		qcvm->global_vars[i].name = LITTLE32(qcvm->global_vars[i].name);
	}

	/* globals */
	qcvm->num_globals = qcvm->header.num_globals;
	qcvm->globals = (union qcvm_global *)((uint8_t *)qcvm->progs + qcvm->header.ofs_globals);

	/* fixup endianness */
	for (i = 0; i < qcvm->num_globals; i++)
	{
		qcvm->globals[i].ui = LITTLE32(qcvm->globals[i].ui);
	}

	/* initialize other fields */
	qcvm->stack_depth = qcvm->local_stack_used = 0;

	return QCVM_OK;
}

int qcvm_query_entity_info(qcvm_t *qcvm, size_t *num_fields, size_t *size)
{
	struct qcvm_header *header;

	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (!qcvm->progs)
		return QCVM_INVALID_PROGS;

	header = (struct qcvm_header *)qcvm->progs;

	if (num_fields)
		*num_fields = (size_t)LITTLE32(header->num_entity_fields);

	if (size)
		*size = (size_t)LITTLE32(header->num_entity_fields) * 4;

	return QCVM_OK;
}

const char *qcvm_result_string(int r)
{
	static const char *results[] = {
		"Ok",
		"Unknown error",
		"Unexpected NULL pointer",
		"Function not found",
		"Invalid opcode",
		"Invalid function",
		"Invalid result code",
		"Builtin call",
		"State call",
		"Builtin not found",
		"Execution finished",
		"Invalid progs data",
		"Unsupported progs version",
		"Unsupported opcode",
		"Unsupported function",
		"Stack overflow",
		"Stack underflow",
		"Argument index is out of range",
		"No tempstrings buffer found",
		"No entities buffer found"
	};

	if (r < 0 || r >= QCVM_NUM_RESULT_CODES)
		return results[QCVM_INVALID_RESULT_CODE];

	return results[r];
}

static const char *str_ofs(qcvm_t *qcvm, int32_t s)
{
	if (s < 0)
	{
		/* invert it */
		s = s * -1;

		/* if its out of range, return the "null" string */
		if (s >= (int32_t)qcvm->len_tempstrings)
			return &qcvm->tempstrings[0];

		return &qcvm->tempstrings[s];
	}
	else
	{
		/* if its out of range, return the "null" string */
		if (s >= (int32_t)qcvm->len_strings)
			return &qcvm->strings[0];

		return &qcvm->strings[s];
	}
}

/* find function by name string */
static int find_function(qcvm_t *qcvm, const char *name, uint32_t *out)
{
	uint32_t i;

	if (!qcvm || !name)
		return QCVM_NULL_POINTER;

	for (i = 0; i < qcvm->num_functions; i++)
	{
		if (QCVM_STRCMP(name, str_ofs(qcvm, qcvm->functions[i].ofs_name)) == 0)
		{
			if (out) *out = i;
			return QCVM_OK;
		}
	}

	return QCVM_FUNCTION_NOT_FOUND;
}

/* setup function for execution */
static int setup_function(qcvm_t *qcvm, struct qcvm_function *func)
{
	int i, x, p;

	if (!qcvm || !func)
		return QCVM_NULL_POINTER;

	/* setup stack */
	qcvm->stack[qcvm->stack_depth] = qcvm->xstack;
	qcvm->stack_depth++;
	if (qcvm->stack_depth >= QCVM_STACK_DEPTH)
		return QCVM_STACK_OVERFLOW;

	/* setup current local stack */
	for (i = 0; i < func->num_locals; i++)
		qcvm->local_stack[qcvm->local_stack_used + i] = qcvm->globals[func->first_parm + i].i;
	qcvm->local_stack_used += func->num_locals;

	/* copy parameters */
	p = func->first_parm;
	for (i = 0; i < func->num_parms; i++)
	{
		for (x = 0; x < func->parm_sizes[i]; x++)
		{
			qcvm->globals[p].i = qcvm->globals[OFS_PARM0 + i * 3 + x].i;
			p++;
		}
	}

	/* finish setting up */
	qcvm->current_function = func;
	qcvm->xstack.function = func;
	qcvm->current_statement_index = func->first_statement - 1;

	return QCVM_OK;
}

static int close_function(qcvm_t *qcvm)
{
	int i, num_locals;

	/* check for stack underflow */
	if (qcvm->stack_depth <= 0)
		return QCVM_STACK_UNDERFLOW;

	num_locals = qcvm->xstack.function->num_locals;
	qcvm->local_stack_used -= num_locals;

	/* check for stack underflow */
	if (qcvm->local_stack_used < 0)
		return QCVM_STACK_UNDERFLOW;

	/* restore locals from the stack */
	for (i = 0; i < num_locals; i++)
		qcvm->globals[qcvm->xstack.function->first_parm + i].i = qcvm->local_stack[qcvm->local_stack_used + i];

	/* move down the stack */
	qcvm->stack_depth--;
	qcvm->xstack.function = qcvm->stack[qcvm->stack_depth].function;

	/* set next statement */
	qcvm->current_statement_index = qcvm->stack[qcvm->stack_depth].statement;

	return QCVM_OK;
}

int qcvm_load(qcvm_t *qcvm, const char *name)
{
	int r;
	uint32_t func;

	if (!qcvm || !name)
		return QCVM_NULL_POINTER;

	/* retrieve function id */
	if ((r = find_function(qcvm, name, &func)) != QCVM_OK)
		return r;

	/* do actual setup */
	return setup_function(qcvm, &qcvm->functions[func]);
}

int qcvm_step(qcvm_t *qcvm)
{
	int r;
	uint16_t opcode;

	if (!qcvm)
		return QCVM_NULL_POINTER;

	/* advance statement */
	qcvm->current_statement_index++;

	/* get current statement and evaluators */
	qcvm->current_statement = &qcvm->statements[qcvm->current_statement_index];
	qcvm->eval[0] = (union qcvm_eval *)&qcvm->globals[qcvm->current_statement->vars[0]];
	qcvm->eval[1] = (union qcvm_eval *)&qcvm->globals[qcvm->current_statement->vars[1]];
	qcvm->eval[2] = (union qcvm_eval *)&qcvm->globals[qcvm->current_statement->vars[2]];

	/* update stack */
	qcvm->xstack.function->profile++;
	qcvm->xstack.statement = qcvm->current_statement_index;

	/* parse opcode */
	opcode = qcvm->current_statement->opcode;
	switch (opcode)
	{
		/* function call */
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
			/* check for null function */
			if (qcvm->eval[0]->func < 1)
				return QCVM_INVALID_FUNCTION;

			/* get function argc */
			qcvm->current_argc = opcode - OPCODE_CALL0;

			/* assign next function value */
			qcvm->next_function = &qcvm->functions[qcvm->eval[0]->func];

			/* setup state for builtin call */
			if (qcvm->next_function->first_statement < 1)
				return QCVM_BUILTIN_CALL;

			/* setup for execution */
			if ((r = setup_function(qcvm, qcvm->next_function) != QCVM_OK))
				return r;

			return QCVM_OK;
		}

		/* function return */
		case OPCODE_RETURN:
		case OPCODE_DONE:
		{
			qcvm->globals[OFS_RETURN] = qcvm->globals[qcvm->current_statement->vars[0]];
			qcvm->globals[OFS_RETURN + 1] = qcvm->globals[qcvm->current_statement->vars[0] + 1];
			qcvm->globals[OFS_RETURN + 2] = qcvm->globals[qcvm->current_statement->vars[0] + 2];

			if ((r = close_function(qcvm)) != QCVM_OK)
				return r;

			if (qcvm->stack_depth == qcvm->exit_depth)
				return QCVM_EXECUTION_FINISHED;

			return QCVM_OK;
		}

		case OPCODE_MUL_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f * qcvm->eval[1]->f;
			break;
		}

		case OPCODE_MUL_V:
		{
			qcvm->eval[2]->f =
				qcvm->eval[0]->v[0] * qcvm->eval[1]->v[0] +
				qcvm->eval[0]->v[1] * qcvm->eval[1]->v[1] +
				qcvm->eval[0]->v[2] * qcvm->eval[1]->v[2];
			break;
		}

		case OPCODE_MUL_FV:
		{
			qcvm->eval[2]->v[0] = qcvm->eval[0]->f * qcvm->eval[1]->v[0];
			qcvm->eval[2]->v[1] = qcvm->eval[0]->f * qcvm->eval[1]->v[1];
			qcvm->eval[2]->v[2] = qcvm->eval[0]->f * qcvm->eval[1]->v[2];
			break;
		}

		case OPCODE_MUL_VF:
		{
			qcvm->eval[2]->v[0] = qcvm->eval[1]->f * qcvm->eval[0]->v[0];
			qcvm->eval[2]->v[1] = qcvm->eval[1]->f * qcvm->eval[0]->v[1];
			qcvm->eval[2]->v[2] = qcvm->eval[1]->f * qcvm->eval[0]->v[2];
			break;
		}

		case OPCODE_DIV_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f / qcvm->eval[1]->f;
			break;
		}

		case OPCODE_ADD_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f + qcvm->eval[1]->f;
			break;
		}

		case OPCODE_ADD_V:
		{
			qcvm->eval[2]->v[0] = qcvm->eval[0]->v[0] + qcvm->eval[1]->v[0];
			qcvm->eval[2]->v[1] = qcvm->eval[0]->v[1] + qcvm->eval[1]->v[1];
			qcvm->eval[2]->v[2] = qcvm->eval[0]->v[2] + qcvm->eval[1]->v[2];
			break;
		}

		case OPCODE_SUB_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f - qcvm->eval[1]->f;
			break;
		}

		case OPCODE_SUB_V:
		{
			qcvm->eval[2]->v[0] = qcvm->eval[0]->v[0] - qcvm->eval[1]->v[0];
			qcvm->eval[2]->v[1] = qcvm->eval[0]->v[1] - qcvm->eval[1]->v[1];
			qcvm->eval[2]->v[2] = qcvm->eval[0]->v[2] - qcvm->eval[1]->v[2];
			break;
		}

		case OPCODE_EQ_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f == qcvm->eval[1]->f;
			break;
		}

		case OPCODE_EQ_V:
		{
			qcvm->eval[2]->f =
				(qcvm->eval[0]->v[0] == qcvm->eval[1]->v[0]) &&
				(qcvm->eval[0]->v[1] == qcvm->eval[1]->v[1]) &&
				(qcvm->eval[0]->v[2] == qcvm->eval[1]->v[2]);
			break;
		}

		case OPCODE_EQ_S:
		{
			qcvm->eval[2]->f = !QCVM_STRCMP(str_ofs(qcvm, qcvm->eval[0]->s), str_ofs(qcvm, qcvm->eval[1]->s));
			break;
		}

		case OPCODE_EQ_E:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->i == qcvm->eval[1]->i;
			break;
		}

		case OPCODE_EQ_FNC:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->func == qcvm->eval[1]->func;
			break;
		}

		case OPCODE_NE_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f != qcvm->eval[1]->f;
			break;
		}

		case OPCODE_NE_V:
		{
			qcvm->eval[2]->f =
				(qcvm->eval[0]->v[0] != qcvm->eval[1]->v[0]) ||
				(qcvm->eval[0]->v[1] != qcvm->eval[1]->v[1]) ||
				(qcvm->eval[0]->v[2] != qcvm->eval[1]->v[2]);
			break;
		}

		case OPCODE_NE_S:
		{
			qcvm->eval[2]->f = QCVM_STRCMP(str_ofs(qcvm, qcvm->eval[0]->s), str_ofs(qcvm, qcvm->eval[1]->s));
			break;
		}

		case OPCODE_NE_E:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->i != qcvm->eval[1]->i;
			break;
		}

		case OPCODE_NE_FNC:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->func != qcvm->eval[1]->func;
			break;
		}

		case OPCODE_LE:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f <= qcvm->eval[1]->f;
			break;
		}

		case OPCODE_GE:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f >= qcvm->eval[1]->f;
			break;
		}

		case OPCODE_LT:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f < qcvm->eval[1]->f;
			break;
		}

		case OPCODE_GT:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f > qcvm->eval[1]->f;
			break;
		}

		case OPCODE_LOAD_F:
		case OPCODE_LOAD_S:
		case OPCODE_LOAD_ENT:
		case OPCODE_LOAD_FLD:
		case OPCODE_LOAD_FNC:
		{
			/* get offset to field */
			qcvm->eval[0] = (union qcvm_eval *)FIELD_PTR(qcvm->eval[0]->e, qcvm->eval[1]->i);

			/* load field value */
			qcvm->eval[2]->i = qcvm->eval[0]->i;

			break;
		}

		case OPCODE_LOAD_V:
		{
			/* get offset to field */
			qcvm->eval[0] = (union qcvm_eval *)FIELD_PTR(qcvm->eval[0]->e, qcvm->eval[1]->i);

			/* load field value */
			qcvm->eval[2]->v[0] = qcvm->eval[0]->v[0];
			qcvm->eval[2]->v[1] = qcvm->eval[0]->v[1];
			qcvm->eval[2]->v[2] = qcvm->eval[0]->v[2];

			break;
		}

		case OPCODE_ADDRESS:
		{
			/* get offset to entity field */
			uint32_t *field_ptr = FIELD_PTR(qcvm->eval[0]->e, qcvm->eval[1]->i);

			/* get offset from start of entities buffer */
			qcvm->eval[2]->i = (int32_t)((uint8_t *)field_ptr - (uint8_t *)qcvm->entities);
			break;
		}

		case OPCODE_STORE_F:
		case OPCODE_STORE_S:
		case OPCODE_STORE_ENT:
		case OPCODE_STORE_FLD:
		case OPCODE_STORE_FNC:
		{
			qcvm->eval[1]->i = qcvm->eval[0]->i;
			break;
		}

		case OPCODE_STORE_V:
		{
			qcvm->eval[1]->v[0] = qcvm->eval[0]->v[0];
			qcvm->eval[1]->v[1] = qcvm->eval[0]->v[1];
			qcvm->eval[1]->v[2] = qcvm->eval[0]->v[2];
			break;
		}

		case OPCODE_STOREP_F:
		case OPCODE_STOREP_S:
		case OPCODE_STOREP_ENT:
		case OPCODE_STOREP_FLD:
		case OPCODE_STOREP_FNC:
		{
			union qcvm_eval *temp = (union qcvm_eval *)((uint8_t *)qcvm->entities + qcvm->eval[1]->i);
			temp->i = qcvm->eval[0]->i;
			break;
		}

		case OPCODE_STOREP_V:
		{
			union qcvm_eval *temp = (union qcvm_eval *)((uint8_t *)qcvm->entities + qcvm->eval[1]->i);
			temp->v[0] = qcvm->eval[0]->v[0];
			temp->v[1] = qcvm->eval[0]->v[1];
			temp->v[2] = qcvm->eval[0]->v[2];
			break;
		}

		case OPCODE_NOT_F:
		{
			qcvm->eval[2]->f = !qcvm->eval[0]->f;
			break;
		}

		case OPCODE_NOT_V:
		{
			qcvm->eval[2]->f = !qcvm->eval[0]->v[0] && !qcvm->eval[0]->v[1] && !qcvm->eval[0]->v[2];
			break;
		}

		case OPCODE_NOT_S:
		{
			qcvm->eval[2]->f = !qcvm->eval[0]->s || !qcvm->strings[qcvm->eval[0]->s];
			break;
		}

		case OPCODE_NOT_ENT:
		{
			qcvm->eval[2]->f = !qcvm->eval[0]->e;
			break;
		}

		case OPCODE_NOT_FNC:
		{
			qcvm->eval[2]->f = !qcvm->eval[0]->func;
			break;
		}

		case OPCODE_IF:
		{
			if (qcvm->eval[0]->i)
				qcvm->current_statement_index += qcvm->current_statement->vars[1] - 1;
			break;
		}

		case OPCODE_IFNOT:
		{
			if (!qcvm->eval[0]->i)
				qcvm->current_statement_index += qcvm->current_statement->vars[1] - 1;
			break;
		}

		case OPCODE_STATE:
		{
			/* must be handled by user program... somehow */
			return QCVM_STATE_CALL;
		}

		case OPCODE_GOTO:
		{
			qcvm->current_statement_index += qcvm->current_statement->vars[0] - 1;
			break;
		}

		case OPCODE_AND_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f && qcvm->eval[1]->f;
			break;
		}

		case OPCODE_OR_F:
		{
			qcvm->eval[2]->f = qcvm->eval[0]->f || qcvm->eval[1]->f;
			break;
		}

		case OPCODE_BITAND_F:
		{
			qcvm->eval[2]->f = (int)qcvm->eval[0]->f & (int)qcvm->eval[1]->f;
			break;
		}

		case OPCODE_BITOR_F:
		{
			qcvm->eval[2]->f = (int)qcvm->eval[0]->f | (int)qcvm->eval[1]->f;
			break;
		}

		default:
		{
			return QCVM_INVALID_OPCODE;
		}
	}

	return QCVM_OK;
}

int qcvm_run(qcvm_t *qcvm, const char *name)
{
	int r, running;
	uint32_t i;

	if (!qcvm || !name)
		return QCVM_NULL_POINTER;

	/* save exit depth */
	qcvm->exit_depth = qcvm->stack_depth;

	/* load function */
	if ((r = qcvm_load(qcvm, name)) != QCVM_OK)
		return r;

	/* start stepping through function */
	r = QCVM_OK;
	running = 1;
	while (running)
	{
		r = qcvm_step(qcvm);

		switch (r)
		{
			/* step went ok, proceed to next */
			case QCVM_OK:
				break;

			/* parse builtin call */
			case QCVM_BUILTIN_CALL:
			{
				if (qcvm->next_function->first_statement == 0)
				{
					const char *name = str_ofs(qcvm, qcvm->next_function->ofs_name);

					/* search for named builtin */
					for (i = 0; i < qcvm->num_builtins; i++)
					{
						if (QCVM_STRCMP(name, qcvm->builtins[i].name) == 0)
						{
							qcvm->builtins[i].func(qcvm, qcvm->builtins[i].user);
							qcvm->next_function->first_statement = -1 * (i + 1);
							r = QCVM_OK;
							break;
						}
					}
				}
				else if (qcvm->next_function->first_statement < 0)
				{
					/* get builtin by index */
					int builtin = (-1 * qcvm->next_function->first_statement) - 1;

					/* check bounds */
					if (builtin < 0 || builtin >= (int)qcvm->num_builtins)
					{
						r = QCVM_BUILTIN_NOT_FOUND;
					}
					else
					{
						qcvm->builtins[builtin].func(qcvm, qcvm->builtins[builtin].user);
						r = QCVM_OK;
					}
				}

				/* its error */
				if (r != QCVM_OK)
					return r;
				else
					break;
			}

			/* parse state call */
			case QCVM_STATE_CALL:
				if (qcvm->state_callback)
					if ((r = qcvm->state_callback(qcvm, qcvm->eval[0]->f, qcvm->eval[1]->func, qcvm->state_callback_user)) != QCVM_OK)
						return r;
				break;

			/* execution finished */
			case QCVM_EXECUTION_FINISHED:
				return QCVM_OK;

			/* something else happened */
			default:
				return r;
		}
	}

	return r;
}

int qcvm_return_string(qcvm_t *qcvm, const char *s)
{
	char *tempstrings_end;
	size_t len;

	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (!qcvm->tempstrings || !qcvm->tempstrings_ptr)
		return QCVM_UNSUPPORTED_FUNCTION;

	/* end of tempstrings buffer */
	tempstrings_end = qcvm->tempstrings + qcvm->len_tempstrings;

	/* get string len */
	len = QCVM_STRLEN(s);

	/* check tempstrings bounds */
	if (qcvm->tempstrings_ptr + len >= tempstrings_end)
		qcvm->tempstrings_ptr = qcvm->tempstrings + 1;

	/* return offset */
	qcvm->globals[OFS_RETURN].i = -1 * (int32_t)(qcvm->tempstrings_ptr - qcvm->tempstrings);
	qcvm->globals[OFS_RETURN + 1].i = 0;
	qcvm->globals[OFS_RETURN + 2].i = 0;

	/* bootleg strncpy */
	while (*s != '\0' && qcvm->tempstrings_ptr < tempstrings_end)
		*qcvm->tempstrings_ptr++ = *s++;

	*qcvm->tempstrings_ptr = '\0';

	return QCVM_OK;
}

int qcvm_return_float(qcvm_t *qcvm, float f)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	qcvm->globals[OFS_RETURN].f = f;
	qcvm->globals[OFS_RETURN + 1].f = 0;
	qcvm->globals[OFS_RETURN + 2].f = 0;

	return QCVM_OK;
}

int qcvm_return_vector(qcvm_t *qcvm, float x, float y, float z)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	qcvm->globals[OFS_RETURN].f = x;
	qcvm->globals[OFS_RETURN + 1].f = y;
	qcvm->globals[OFS_RETURN + 2].f = z;

	return QCVM_OK;
}

int qcvm_return_entity(qcvm_t *qcvm, uint32_t e)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	qcvm->globals[OFS_RETURN].ui = e;
	qcvm->globals[OFS_RETURN + 1].ui = 0;
	qcvm->globals[OFS_RETURN + 2].ui = 0;

	return QCVM_OK;
}

int qcvm_query_argument_count(qcvm_t *qcvm, int *argc)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (argc)
		*argc = qcvm->current_argc;

	return QCVM_OK;
}

int qcvm_get_return_float(qcvm_t *qcvm, float *f)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (f)
		*f = qcvm->globals[OFS_RETURN].f;

	return QCVM_OK;
}

int qcvm_get_return_vector(qcvm_t *qcvm, float *x, float *y, float *z)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (x)
		*x = qcvm->globals[OFS_RETURN].f;

	if (y)
		*y = qcvm->globals[OFS_RETURN + 1].f;

	if (z)
		*z = qcvm->globals[OFS_RETURN + 2].f;

	return QCVM_OK;
}

int qcvm_get_argument_string(qcvm_t *qcvm, int i, const char **s)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (i < 0 || i >= 8)
		return QCVM_ARGUMENT_OUT_OF_RANGE;

	if (s)
		*s = str_ofs(qcvm, qcvm->globals[OFS_PARM0 + (i * 3)].i);

	return QCVM_OK;
}

int qcvm_get_argument_float(qcvm_t *qcvm, int i, float *f)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (i < 0 || i >= 8)
		return QCVM_ARGUMENT_OUT_OF_RANGE;

	if (f)
		*f = qcvm->globals[OFS_PARM0 + (i * 3)].f;

	return QCVM_OK;
}

int qcvm_get_argument_int(qcvm_t *qcvm, int i, int *n)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (i < 0 || i >= 8)
		return QCVM_ARGUMENT_OUT_OF_RANGE;

	if (n)
		*n = qcvm->globals[OFS_PARM0 + (i * 3)].i;

	return QCVM_OK;
}

int qcvm_get_argument_vector(qcvm_t *qcvm, int i, float *x, float *y, float *z)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (i < 0 || i >= 8)
		return QCVM_ARGUMENT_OUT_OF_RANGE;

	if (x)
		*x = qcvm->globals[OFS_PARM0 + (i * 3)].f;

	if (y)
		*y = qcvm->globals[OFS_PARM0 + (i * 3) + 1].f;

	if (z)
		*z = qcvm->globals[OFS_PARM0 + (i * 3) + 2].f;

	return QCVM_OK;
}

int qcvm_get_argument_entity(qcvm_t *qcvm, int i, uint32_t *e)
{
	if (!qcvm)
		return QCVM_NULL_POINTER;

	if (i < 0 || i >= 8)
		return QCVM_ARGUMENT_OUT_OF_RANGE;

	if (e)
		*e = qcvm->globals[OFS_PARM0 + (i * 3)].ui;

	return QCVM_OK;
}

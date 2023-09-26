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

/* guard */
#pragma once
#ifndef _QCVM_PRIVATE_H_
#define _QCVM_PRIVATE_H_
#ifdef __cplusplus
extern "C" {
#endif

/* include public header */
#include "qcvm.h"

/* allocate tempstrings table */
#ifndef ALLOCATE_TEMPSTRINGS
#define ALLOCATE_TEMPSTRINGS 1
#endif

#ifndef TEMPSTRINGS_SIZE
#define TEMPSTRINGS_SIZE 2048UL
#endif

/* allocate entity table */
#ifndef ALLOCATE_ENTITIES
#define ALLOCATE_ENTITIES 1
#endif

#ifndef NUM_ENTITIES
#define NUM_ENTITIES 128UL
#endif

#ifndef ENTITY_SIZE
#define ENTITY_SIZE 512UL
#endif

/* allocate exports table */
#ifndef ALLOCATE_EXPORTS
#define ALLOCATE_EXPORTS 1
#endif

#ifndef NUM_EXPORTS
#define NUM_EXPORTS 256UL
#endif

/* offsets into the global table */
#define OFS_NULL			0
#define OFS_RETURN			1
#define OFS_PARM0			4
#define OFS_PARM1			7
#define OFS_PARM2			10
#define OFS_PARM3			13
#define OFS_PARM4			16
#define OFS_PARM5			19
#define OFS_PARM6			22
#define OFS_PARM7			25
#define OFS_RESERVED		28

/* stack */
#define	STACK_DEPTH			32
#define	LOCAL_STACK_DEPTH	2048

/* get string from offset */
#define GET_STRING_OFS(o) (qcvm->strings + (o))

/* retrieve values from the globals table */
#define GET_FLOAT(o) (qcvm->globals[o])
#define GET_INT(o) (*(int *)&qcvm->globals[o])
#define GET_VECTOR(o) (&qcvm->globals[o])
#define GET_STRING(o) (qcvm->strings + *(int *)&qcvm->globals[o])
#define GET_FUNCTION(o) (*(int *)&qcvm->globals[o])
#define GET_ENTITY(o) (*(int *)&qcvm->globals[o])

#define	ENTITY_TO_QC(e) ((unsigned char *)(e) - (unsigned char *)qcvm->entities)
#define QC_TO_ENTITY(e) ((qcvm_entity_t *)((unsigned char *)qcvm->entities + (e)))

/* retrieve values from entity fields */
#define FIELD_FLOAT(e, f) (((float *)qcvm->entities[e].v)[f])
#define FIELD_INT(e, f) (((int *)qcvm->entities[e].v)[f])

/* return from function */
#define RETURN_FLOAT(a) (GET_FLOAT(OFS_RETURN) = (a))
#define RETURN_INT(a) (GET_INT(OFS_RETURN) = (a))
#define RETURN_STRING(a) (GET_INT(OFS_RETURN) = (a) - qcvm->strings)
#define	RETURN_ENTITY(e) (GET_INT(OFS_RETURN) = ENTITY_TO_QC(e))
#define RETURN_VECTOR(a, b, c) \
	GET_FLOAT(OFS_RETURN) = (a); \
	GET_FLOAT(OFS_RETURN + 1) = (b); \
	GET_FLOAT(OFS_RETURN + 2) = (c);

/* small typedefs to make it easier to read certain parts of the code */
typedef int qcvm_string_t;
typedef float qcvm_global_t;

/* version macros */
#define PROGS_VERSION_ID 6
#define PROGS_VERSION_FTE 7

/*
 *
 * qc progs.dat header.
 *
 * the first section of a compiled qc object will contain this header.
 * it contains all the appropriate offsets for functions, statements,
 * fields, strings, globals, etc.
 *
 */
typedef struct qcvm_header_t
{
	int version;			/* file version */
	int crc;				/* compiled crc */
	int ofs_statements;		/* offset to statements */
	int num_statements;		/* number of statements */
	int ofs_global_vars;	/* offset to global vars */
	int num_global_vars;	/* number of global vars */
	int ofs_field_vars;		/* offset to field vars */
	int num_field_vars;		/* number of field vars */
	int ofs_functions;		/* offset to functions */
	int num_functions;		/* number of functions */
	int ofs_strings;		/* offset to string table */
	int len_strings;		/* size of string table */
	int ofs_globals;		/* offset to globals */
	int num_globals;		/* number of globals */
	int num_entity_fields;	/* number of entity fields */

	/* version 7 fields */
	int ofs_files;
	int ofs_linenums;
	int ofs_compressed_functions;
	int num_compressed_functions;
	int ofs_types;
	int num_types;
	int num_compressed_blocks;
	char secondary_version[4];
} qcvm_header_t;

/*
 *
 * qc variable definition structure.
 *
 * contains an offset to the stored value, and a short
 * defining what kind of variable it is. also contains an offset
 * into the string table for its name.
 *
 */
typedef struct qcvm_var_t
{
	unsigned short type;
	unsigned short ofs;
	qcvm_string_t name;
} qcvm_var_t;

/*
 *
 * qc statement structure.
 *
 * contains an opcode and 3 arguments, which are indexes into
 * the globals buffer.
 *
 */
typedef struct qcvm_statement_t
{
	unsigned short opcode;
	short vars[3];
} qcvm_statement_t;

/*
 *
 * qc function structure.
 *
 * contains information on function parameters, offset to the name string
 * and the name of the file the function originated from.
 *
 */
typedef struct qcvm_function_t
{
	int first_statement;
	int first_parm;
	int num_locals;
	int profile;
	qcvm_string_t name;
	qcvm_string_t file;
	int num_parms;
	unsigned char parm_sizes[8];
} qcvm_function_t;

/*
 *
 * qc evaluator union.
 *
 * used by the qc runtime to parse a statement and evaluate its contents.
 *
 * should always be 12 bytes in size.
 *
 * it can contain:
 * - offset into string table
 * - float value
 * - 3-part float vector
 * - index into function table
 * - integer value
 * - index into entity table
 *
 */
typedef union qcvm_evaluator_t
{
	int string;			/* offset into string table */
	float float_;		/* float value */
	float vector[3];	/* 3-part float vector */
	int function;		/* index into function table */
	int int_;			/* integer value */
	int entity;			/* index into entity table */
} qcvm_evaluator_t;

/*
 *
 * qc stack value structure.
 *
 * contains a pointer to a function in the function table,
 * and a statement id.
 *
 */
typedef struct qcvm_stack_t
{
	int statement;
	qcvm_function_t *function;
} qcvm_stack_t;

/*
 *
 * generic entity container.
 *
 * contains no useful information in it of itself, except to hold data
 * assigned to it from fields in the qc.
 *
 */
typedef struct qcvm_entity_t
{
	unsigned char v[ENTITY_SIZE];
} qcvm_entity_t;

/*
 *
 * the main qc runtime structure.
 *
 * contains a buffer with the loaded progs.dat memory,
 * as well as pointers to specific locations in that memory
 * for running functions, manipulating & storing globals, etc.
 *
 * everything in this struct should be considered read-only.
 *
 */
struct qcvm_runtime
{
	/* functions */
	int function_argc;				/* number of args in current function */

	/* miscellaneous */
	int trace;						/* print trace info */
	int len_entity;					/* size of the entity struct */

	/* stack */
	qcvm_stack_t stack[32];			/* stack buffer */
	int stack_depth;				/* current depth into stack buffer */
	int local_stack[2048];			/* local stack */
	int local_stack_used;			/* amount of local stack used */
	qcvm_stack_t xstack;			/* stack temp */

	/* pointers */
	qcvm_header_t *header;			/* pointer to header */
	qcvm_statement_t *statements;	/* pointer to statements */
	qcvm_function_t *functions;		/* pointer to functions */
	char *strings;					/* pointer to string table */

	#if ALLOCATE_TEMPSTRINGS
	char *tempstrings;				/* pointer to tempstrings table */
	char *tempstrings_ptr;			/* pointer to current tempstrings offset */
	#endif

	#if ALLOCATE_EXPORTS
	qcvm_export_t *exports;			/* exports table */
	int num_exports;
	#endif

	qcvm_var_t *field_vars;			/* pointer to field vars */
	qcvm_var_t *global_vars;		/* pointer to global vars */
	qcvm_global_t *globals;			/* pointer to globals table */
	qcvm_entity_t *entities;		/* pointer to entities buffer */
	int num_entities;

	/* runtime */
	qcvm_evaluator_t *eval_p[4];
	int exit_depth;
	int statement_i;
	int export_i;
	int fail;
	int done;
	qcvm_statement_t *statement_p;
	qcvm_function_t *function_p;
	qcvm_function_t *nextfunction_p;
	qcvm_entity_t *entity_p;

	/* memory pool */
	void *pool;						/* pointer to memory pool */
	int len_pool;					/* size of memory pool */
};

/* qcvm opcodes */
#define QCVM_OPCODE(o) OPCODE_##o,
typedef enum qcvm_opcode_t
{
	#include "qcvm_opcodes.h"
	QCVM_OPCODE(MAX)
} qcvm_opcode_t;
#undef QCVM_OPCODE

/* qcvm opcode function */
typedef void qcvm_opcode_func_t(qcvm_t *qcvm);

/* qcvm opcode function table entry */
typedef struct qcvm_opcode_table_entry_t
{
	/* function to execute */
	qcvm_opcode_func_t *func;

	/* integer value of opcode */
	qcvm_opcode_t op;

	/* printable name of opcode */
	const char *name;
} qcvm_opcode_table_entry_t;

/* qcvm opcode function table */
extern qcvm_opcode_table_entry_t qcvm_opcode_table[];

/* guard */
#ifdef __cplusplus
}
#endif
#endif /* _QCVM_PRIVATE_H_ */

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
 * File:			/source/qcvm.h
 *
 * Description:		QCVM main header
 *
 * Last Modified:	February 6th 2023
 *
 *****************************************************************************/

/*
 * headers
 */

/* std */
#include <stdint.h>

/*
 * macros
 */

/* max builtins */
#ifndef QC_MAX_BUILTINS
#define QC_MAX_BUILTINS			256
#endif

/* max exports */
#ifndef QC_MAX_EXPORTS
#define QC_MAX_EXPORTS			256
#endif

/* tempstring legth */
#ifndef QC_TEMPSTRING_LENGTH
#define QC_TEMPSTRING_LENGTH	128
#endif

/* true/false */
#define QC_TRUE					1
#define QC_FALSE				0

/* version */
#define	QC_VERSION				6

/* parameters */
#define	QC_MAX_PARMS			8

/* global offsets */
#define QC_OFS_NULL				0
#define QC_OFS_RETURN			1
#define QC_OFS_PARM0			4
#define QC_OFS_PARM1			7
#define QC_OFS_PARM2			10
#define QC_OFS_PARM3			13
#define QC_OFS_PARM4			16
#define QC_OFS_PARM5			19
#define QC_OFS_PARM6			22
#define QC_OFS_PARM7			25
#define QC_OFS_RESERVED			28

/* stack */
#define	QC_MAX_STACK_DEPTH		32
#define	QC_LOCALSTACK_SIZE		2048

/*
 * types
 */

/* callback to c function */
typedef void (*qc_builtin_t)(void);

/* types for a builtin parameter */
typedef enum qc_parm_type_t
{
	QC_PARM_NULL,
	QC_PARM_STRING,
	QC_PARM_FLOAT,
	QC_PARM_VECTOR,
	QC_PARM_FUNCTION,
	QC_PARM_INT,
	QC_PARM_EDICT
} qc_parm_type_t;

/* c exports */
typedef struct qc_export_t
{
	const char *name;
	const char *desc;
	qc_builtin_t function;
	/*
	qc_parm_type_t parms[QC_MAX_PARMS];
	const char *parm_strings[QC_MAX_PARMS];
	*/
} qc_export_t;

/* statement evaluation */
typedef union qc_eval_t
{
	int32_t string;
	float _float;
	float vector[3];
	int32_t function;
	int32_t _int;
	int32_t edict;
} qc_eval_t;

/* statement */
typedef struct qc_statement_t
{
	uint16_t op;
	int16_t a, b, c;
} qc_statement_t;

/* variable definition */
typedef struct qc_def_t
{
	uint16_t type;
	uint16_t ofs;
	int32_t s_name;
} qc_def_t;

/* function */
typedef struct qc_function_t
{
	int32_t first_statement;
	int32_t parm_start;
	int32_t locals;
	int32_t profile;
	int32_t s_name;
	int32_t s_file;
	int32_t numparms;
	uint8_t parm_size[QC_MAX_PARMS];
} qc_function_t;

/* main struct type */
typedef struct qc_t
{
	int32_t version;
	int32_t crc;
	int32_t ofs_statements;
	int32_t num_statements;
	int32_t ofs_globaldefs;
	int32_t num_globaldefs;
	int32_t ofs_fielddefs;
	int32_t num_fielddefs;
	int32_t ofs_functions;
	int32_t num_functions;
	int32_t ofs_strings;
	int32_t num_strings;
	int32_t ofs_globals;
	int32_t num_globals;
	int32_t entityfields;
} qc_t;

/* stack type */
typedef struct qc_stack_t
{
	int s;
	qc_function_t *f;
} qc_stack_t;

/* opcodes */
typedef enum qc_opcodes_t
{
	QC_OP_DONE,
	QC_OP_MUL_F,
	QC_OP_MUL_V,
	QC_OP_MUL_FV,
	QC_OP_MUL_VF,
	QC_OP_DIV_F,
	QC_OP_ADD_F,
	QC_OP_ADD_V,
	QC_OP_SUB_F,
	QC_OP_SUB_V,

	QC_OP_EQ_F,
	QC_OP_EQ_V,
	QC_OP_EQ_S,
	QC_OP_EQ_E,
	QC_OP_EQ_FNC,

	QC_OP_NE_F,
	QC_OP_NE_V,
	QC_OP_NE_S,
	QC_OP_NE_E,
	QC_OP_NE_FNC,

	QC_OP_LE,
	QC_OP_GE,
	QC_OP_LT,
	QC_OP_GT,

	QC_OP_LOAD_F,
	QC_OP_LOAD_V,
	QC_OP_LOAD_S,
	QC_OP_LOAD_ENT,
	QC_OP_LOAD_FLD,
	QC_OP_LOAD_FNC,

	QC_OP_ADDRESS,

	QC_OP_STORE_F,
	QC_OP_STORE_V,
	QC_OP_STORE_S,
	QC_OP_STORE_ENT,
	QC_OP_STORE_FLD,
	QC_OP_STORE_FNC,

	QC_OP_STOREP_F,
	QC_OP_STOREP_V,
	QC_OP_STOREP_S,
	QC_OP_STOREP_ENT,
	QC_OP_STOREP_FLD,
	QC_OP_STOREP_FNC,

	QC_OP_RETURN,
	QC_OP_NOT_F,
	QC_OP_NOT_V,
	QC_OP_NOT_S,
	QC_OP_NOT_ENT,
	QC_OP_NOT_FNC,
	QC_OP_IF,
	QC_OP_IFNOT,
	QC_OP_CALL0,
	QC_OP_CALL1,
	QC_OP_CALL2,
	QC_OP_CALL3,
	QC_OP_CALL4,
	QC_OP_CALL5,
	QC_OP_CALL6,
	QC_OP_CALL7,
	QC_OP_CALL8,
	QC_OP_STATE,
	QC_OP_GOTO,
	QC_OP_AND,
	QC_OP_OR,

	QC_OP_BITAND,
	QC_OP_BITOR
} qc_opcodes_t;

/* helper macros */
#define QC_GET_FLOAT(o) (qc_globals[o])
#define QC_GET_INT(o) (*(int *)&qc_globals[o])
#define QC_GET_VECTOR(o) (&qc_globals[o])
#define QC_GET_STRING(o) (qc_strings + *(int32_t *)&qc_globals[o])
#define QC_GET_FUNCTION(o) (*(int32_t *)&qc_globals[o])

/* return macros */
#define QC_RETURN_FLOAT(a) (QC_GET_FLOAT(QC_OFS_RETURN) = (a))
#define QC_RETURN_INT(a) (QC_GET_INT(QC_OFS_RETURN) = (a))
#define QC_RETURN_STRING(a) (QC_GET_INT(QC_OFS_RETURN) = (a) - qc_strings)
#define QC_RETURN_VECTOR(a, b, c) \
	QC_GET_FLOAT(QC_OFS_RETURN) = (a); \
	QC_GET_FLOAT(QC_OFS_RETURN + 1) = (b); \
	QC_GET_FLOAT(QC_OFS_RETURN + 2) = (c);

/*
 * globals
 */

/* main */
extern qc_t *qc;
extern qc_function_t *qc_functions;
extern char *qc_strings;
extern qc_def_t *qc_fielddefs;
extern qc_def_t *qc_globaldefs;
extern qc_statement_t *qc_statements;
extern float *qc_globals;
extern int qc_edict_size;

/* stack */
extern qc_stack_t qc_stack[QC_MAX_STACK_DEPTH];
extern int qc_depth;
extern int qc_localstack[QC_LOCALSTACK_SIZE];
extern int qc_localstack_used;

/* trace */
extern int qc_trace;

/* x */
extern qc_function_t *qc_xfunction;
extern int qc_xstatement;

/* args */
extern int qc_argc;

/* builtins */
extern qc_builtin_t qc_builtins[QC_MAX_BUILTINS]; 
extern int qc_num_builtins;

/* strings */
extern char qc_string_temp[QC_TEMPSTRING_LENGTH];

/*
 * functions
 */

/* load progs. returns 0 on success */
int qc_load(const char *filename);

/* exit qc and free memory */
void qc_exit();

/* print an error and exit qc */
void qc_error(char *s, ...);

/* run execution loop */
void qc_execute(int fnum);

/* search for function by name */
int qc_function_get(const char *name);

/* add builtin to stack */
void qc_builtin_add(qc_builtin_t func);

/* set parameter to float */
void qc_set_parm_float(int i, float f);

/* set parameter to vector */
void qc_set_parm_vector(int i, float v0, float v1, float v2);

/* add export */
void qc_export_add(qc_export_t *export);

/* dump exports */
void qc_dump_exports(const char *filename);

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
 * File:			/qcvm/qcvm.h
 *
 * Description:		QCVM main header
 *
 * Last Modified:	February 8th 2023
 *
 *****************************************************************************/

#pragma once
#ifndef _QCVM_H_
#define _QCVM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * macros
 */

/* max exports */
#ifndef QC_MAX_EXPORTS
#define QC_MAX_EXPORTS			256
#endif

/* number of tempstrings */
#ifndef QC_NUM_TEMPSTRINGS
#define QC_NUM_TEMPSTRINGS		8
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

/* main qc types */
typedef enum qc_types_t
{
	QC_TYPE_VOID,
	QC_TYPE_STRING,
	QC_TYPE_FLOAT,
	QC_TYPE_VECTOR,
	QC_TYPE_FUNCTION,
	QC_TYPE_INT,
	QC_TYPE_EDICT,
	QC_TYPE_VARGS
} qc_types_t;

/* qc export parm */
typedef struct qc_parm_t
{
	const char *name;
	qc_types_t type;
} qc_parm_t;

/* c exports */
typedef struct qc_export_t
{
	const char *name;
	const char *desc;
	qc_builtin_t func;
	qc_types_t ret;
	qc_parm_t parms[QC_MAX_PARMS];
	int parmc;
} qc_export_t;

/* statement evaluation */
typedef union qc_eval_t
{
	int string;
	float _float;
	float vector[3];
	int function;
	int _int;
	int edict;
} qc_eval_t;

/* statement */
typedef struct qc_statement_t
{
	unsigned short op;
	short a, b, c;
} qc_statement_t;

/* variable definition */
typedef struct qc_def_t
{
	unsigned short type;
	unsigned short ofs;
	int s_name;
} qc_def_t;

/* function */
typedef struct qc_function_t
{
	int first_statement;
	int parm_start;
	int locals;
	int profile;
	int s_name;
	int s_file;
	int numparms;
	unsigned char parm_size[QC_MAX_PARMS];
} qc_function_t;

/* main struct type */
typedef struct qc_data_t
{
	int version;
	int crc;
	int ofs_statements;
	int num_statements;
	int ofs_globaldefs;
	int num_globaldefs;
	int ofs_fielddefs;
	int num_fielddefs;
	int ofs_functions;
	int num_functions;
	int ofs_strings;
	int num_strings;
	int ofs_globals;
	int num_globals;
	int entityfields;
} qc_data_t;

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

/*
 * helper macros
 */

/* retrieving values from parms */
#define QC_GET_FLOAT(o) (qc_globals[o])
#define QC_GET_INT(o) (*(int *)&qc_globals[o])
#define QC_GET_VECTOR(o) (&qc_globals[o])
#define QC_GET_STRING(o) (qc_strings + *(int *)&qc_globals[o])
#define QC_GET_FUNCTION(o) (*(int *)&qc_globals[o])

/* set return value from function */
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
extern qc_data_t *qc_data;
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

/* strings */
extern char qc_tstrings[QC_NUM_TEMPSTRINGS][QC_TEMPSTRING_LENGTH];
extern int qc_tstring_idx;

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

/* set parameter to float */
void qc_set_parm_float(int i, float f);

/* set parameter to vector */
void qc_set_parm_vector(int i, float v0, float v1, float v2);

/* add export */
void qc_add_export(qc_export_t *export);

/* dump exports */
void qc_dump_exports(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* _QCVM_H_ */

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
#ifndef _QCVM_H_
#define _QCVM_H_
#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 * types
 *
 */

/* qcvm runtime */
typedef struct qcvm_runtime qcvm_t;

/* var type */
typedef enum qcvm_var_type_t
{

	QCVM_VAR_VOID,
	QCVM_VAR_STRING,
	QCVM_VAR_FLOAT,
	QCVM_VAR_VECTOR,
	QCVM_VAR_ENTITY,
	QCVM_VAR_FIELD,
	QCVM_VAR_FUNCTION,
	QCVM_VAR_POINTER

} qcvm_var_type_t;

/* export func */
typedef void (*qcvm_export_func_t)(qcvm_t *);

/* export return type */
typedef enum qcvm_export_type_t
{

	QCVM_VOID,
	QCVM_FLOAT,
	QCVM_INT,
	QCVM_ENTITY,
	QCVM_STRING,
	QCVM_VECTOR,
	QCVM_VARGS

} qcvm_export_type_t;

/* arg for export */
typedef struct qcvm_export_arg_t
{
	char name[32];
	qcvm_export_type_t type;

} qcvm_export_arg_t;

/* export structure */
typedef struct qcvm_export_t
{
	char name[32];
	qcvm_export_func_t func;
	qcvm_export_type_t type;
	int argc;
	qcvm_export_arg_t args[8];

} qcvm_export_t;

/* vector */
typedef struct qcvm_vec3_t
{
	float x, y, z;
} qcvm_vec3_t;

/*
 *
 * functions
 *
 */

/*
 * qcvm_bootstrap.c
 */

/* open progs.dat file and return qcvm runtime handle */
qcvm_t *qcvm_open(const char *filename);

/* destroy qcvm runtime handle */
void qcvm_close(qcvm_t *qcvm);

/* get error */
const char *qcvm_get_error(void);

/*
 * qcvm_entities.c
 */

/* add entity and return id */
int qcvm_add_entity(qcvm_t *qcvm);

/* find global entity by name */
int qcvm_find_entity(qcvm_t *qcvm, const char *name);

/*
 * qcvm_exports.c
 */

/* add an argument to export */
void qcvm_add_export_arg(qcvm_export_t *export, const char *name, qcvm_export_type_t type);

/* create new export */
qcvm_export_t *qcvm_create_export(const char *name, qcvm_export_func_t func, qcvm_export_type_t type);

/* free export */
void qcvm_free_export(qcvm_export_t *export);

/* add export to qcvm */
void qcvm_add_export(qcvm_t *qcvm, qcvm_export_t *export);

/* dump exports to properly formatted qc */
void qcvm_dump_exports(qcvm_t *qcvm, const char *filename);

/*
 * qcvm_fields.c
 */

/* get entity field string */
const char *qcvm_get_field_string(qcvm_t *qcvm, int entity, int field);

/* get entity field vector */
qcvm_vec3_t qcvm_get_field_vector(qcvm_t *qcvm, int entity, int field);

/* get entity field float */
float qcvm_get_field_float(qcvm_t *qcvm, int entity, int field);

/* get entity field int */
int qcvm_get_field_int(qcvm_t *qcvm, int entity, int field);

/* set entity field vector */
void qcvm_set_field_vector(qcvm_t *qcvm, int entity, int field, float a, float b, float c);

/* set entity field float */
void qcvm_set_field_float(qcvm_t *qcvm, int entity, int field, float val);

/* set entity field int */
void qcvm_set_field_int(qcvm_t *qcvm, int entity, int field, int val);

/* find entity field by name */
int qcvm_find_field(qcvm_t *qcvm, const char *name);

/*
 * qcvm_functions.c
 */

/* search all functions in the qcvm and return its function number if found */
int qcvm_find_function(qcvm_t *qcvm, const char *name);

/*
 * qcvm_globals.c
 */

/* set global entity by number */
void qcvm_set_global_entity(qcvm_t *qcvm, int global, int entity);

/* set the global int at the specified offset to the specified value */
void qcvm_set_global_int(qcvm_t *qcvm, int global, int val);

/* set the global float at the specified offset to the specified value */
void qcvm_set_global_float(qcvm_t *qcvm, int global, float val);

/* set the global vector at the specified offset to the specified value */
void qcvm_set_global_vector(qcvm_t *qcvm, int global, float a, float b, float c);

/* retrieve global entity */
int qcvm_get_global_entity(qcvm_t *qcvm, int global);

/* retrieve global int */
int qcvm_get_global_int(qcvm_t *qcvm, int global);

/* retrieve global float */
float qcvm_get_global_float(qcvm_t *qcvm, int global);

/* retrieve global vector */
qcvm_vec3_t qcvm_get_global_vector(qcvm_t *qcvm, int global);

/* retrieve the offset of a global by name */
int qcvm_find_global(qcvm_t *qcvm, const char *name);

/*
 * qcvm_parameters.c
 */

/* set the specified parameter of the next function call to a string */
void qcvm_set_parm_string(qcvm_t *qcvm, int parm, const char *s);

/* set the specified parameter of the next function call to a vector */
void qcvm_set_parm_vector(qcvm_t *qcvm, int parm, float a, float b, float c);

/* set the specified parameter of the next function call to an int */
void qcvm_set_parm_int(qcvm_t *qcvm, int parm, int val);

/* set the specified parameter of the next function call to a float */
void qcvm_set_parm_float(qcvm_t *qcvm, int parm, float val);

/* get the argument count of the most recently called function */
int qcvm_get_argc(qcvm_t *qcvm);

/* retrieve the specified function parameter as an entity id */
int qcvm_get_parm_entity(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as a vector */
qcvm_vec3_t qcvm_get_parm_vector(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as a string */
const char *qcvm_get_parm_string(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as an int */
int qcvm_get_parm_int(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as a float */
float qcvm_get_parm_float(qcvm_t *qcvm, int parm);

/*
 * qcvm_return.c
 */

/* return a new entity */
void qcvm_return_entity(qcvm_t *qcvm, int entity);

/* return a string to the function that called this one */
void qcvm_return_string(qcvm_t *qcvm, const char *s);

/* return a formatted string to the function that called this one */
void qcvm_return_stringf(qcvm_t *qcvm, const char *s, ...);

/* return a vector to the function that called this one */
void qcvm_return_vector(qcvm_t *qcvm, float a, float b, float c);

/* return an int to the function that called this one */
void qcvm_return_int(qcvm_t *qcvm, int val);

/* return a float to the function that called this one */
void qcvm_return_float(qcvm_t *qcvm, float val);

/*
 * qcvm_runtime.c
 */

/* execute qcvm runtime loop */
void qcvm_run(qcvm_t *qcvm, int func);

/* guard */
#ifdef __cplusplus
}
#endif
#endif /* _QCVM_H_ */

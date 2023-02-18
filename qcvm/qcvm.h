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

#pragma once
#ifndef _QCVM_H_
#define _QCVM_H_

/*
 *
 * types
 *
 */

/* qcvm runtime */
typedef struct qcvm_runtime qcvm_t;

/* export */
typedef void (*qcvm_export_t)(qcvm_t *qcvm);

/* vector */
typedef struct qcvm_vec3
{
	float x, y, z;
} qcvm_vec3;

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

/*
 * qcvm_utils.c
 */

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
qcvm_vec3 qcvm_get_global_vector(qcvm_t *qcvm, int global);

/* retrieve the offset of a global by name */
int qcvm_find_global(qcvm_t *qcvm, const char *name);

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
qcvm_vec3 qcvm_get_parm_vector(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as a string */
const char *qcvm_get_parm_string(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as an int */
int qcvm_get_parm_int(qcvm_t *qcvm, int parm);

/* retrieve the specified function parameter as a float */
float qcvm_get_parm_float(qcvm_t *qcvm, int parm);

/* return a new entity */
int qcvm_return_entity(qcvm_t *qcvm);

/* return a string to the function that called this one */
void qcvm_return_string(qcvm_t *qcvm, const char *s);

/* return a vector to the function that called this one */
void qcvm_return_vector(qcvm_t *qcvm, float a, float b, float c);

/* return an int to the function that called this one */
void qcvm_return_int(qcvm_t *qcvm, int val);

/* return a float to the function that called this one */
void qcvm_return_float(qcvm_t *qcvm, float val);

/* search all functions in the qcvm and return its function number if found */
int qcvm_find_function(qcvm_t *qcvm, const char *name);

/* add export to qcvm */
int qcvm_add_export(qcvm_t *qcvm, qcvm_export_t export);

/*
 * qcvm_runtime.c
 */

/* execute qcvm runtime loop */
void qcvm_run(qcvm_t *qcvm, int func);

#endif /* _QCVM_H_ */

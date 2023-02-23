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
#include <string.h>

/* qcvm */
#include "qcvm_private.h"

/*
 * entities
 */

/* find global entity by name */
int qcvm_find_entity(qcvm_t *qcvm, const char *name)
{
	int i;

	for (i = 1; i < qcvm->header->num_global_vars; i++)
	{
		if (qcvm->global_vars[i].type == 32772 && strcmp(name, GET_STRING_OFS(qcvm->global_vars[i].name)) == 0)
			return i;
	}

	return -1;
}

/* add entity and return id */
int qcvm_add_entity(qcvm_t *qcvm)
{
	qcvm->num_entities++;
	return qcvm->num_entities - 1;
}

/*
 * entity fields
 */

/* get entity field string */
const char *qcvm_get_field_string(qcvm_t *qcvm, int entity, int field)
{
	return GET_STRING_OFS(((int *)qcvm->entities[entity].v)[field]);
}

/* get entity field vector */
qcvm_vec3 qcvm_get_field_vector(qcvm_t *qcvm, int entity, int field)
{
	qcvm_vec3 ret;

	ret.x = ((float *)qcvm->entities[entity].v)[field];
	ret.y = ((float *)qcvm->entities[entity].v)[field + 1];
	ret.z = ((float *)qcvm->entities[entity].v)[field + 2];

	return ret;
}

/* get entity field float */
float qcvm_get_field_float(qcvm_t *qcvm, int entity, int field)
{
	return ((float *)qcvm->entities[entity].v)[field];
}

/* get entity field int */
int qcvm_get_field_int(qcvm_t *qcvm, int entity, int field)
{
	return ((int *)qcvm->entities[entity].v)[field];
}

/* set entity field vector */
void qcvm_set_field_vector(qcvm_t *qcvm, int entity, int field, float a, float b, float c)
{
	((float *)qcvm->entities[entity].v)[field] = a;
	((float *)qcvm->entities[entity].v)[field + 1] = b;
	((float *)qcvm->entities[entity].v)[field + 2] = c;
}

/* set entity field float */
void qcvm_set_field_float(qcvm_t *qcvm, int entity, int field, float val)
{
	((float *)qcvm->entities[entity].v)[field] = val;
}

/* set entity field int */
void qcvm_set_field_int(qcvm_t *qcvm, int entity, int field, int val)
{
	((int *)qcvm->entities[entity].v)[field] = val;
}

/* find entity field by name */
int qcvm_find_field(qcvm_t *qcvm, const char *name)
{
	/* variables */
	int i;

	/* search globals */
	for (i = 1; i < qcvm->header->num_field_vars; i++)
	{
		if (strcmp(GET_STRING_OFS(qcvm->field_vars[i].name), name) == 0)
			return qcvm->field_vars[i].ofs;
	}

	/* return failure */
	return -1;
}

/*
 * globals
 */

/* set global entity by number */
void qcvm_set_global_entity(qcvm_t *qcvm, int global, int entity)
{
	GET_ENTITY(global) = entity * sizeof(qcvm_entity_t);
}

/* set global int by number */
void qcvm_set_global_int(qcvm_t *qcvm, int global, int val)
{
	GET_INT(global) = val;
}

/* set global float by number */
void qcvm_set_global_float(qcvm_t *qcvm, int global, float val)
{
	GET_FLOAT(global) = val;
}

/* set global vector by number */
void qcvm_set_global_vector(qcvm_t *qcvm, int global, float a, float b, float c)
{
	GET_FLOAT(global) = a;
	GET_FLOAT(global + 1) = b;
	GET_FLOAT(global + 2) = c;
}

/* retrieve global entity */
int qcvm_get_global_entity(qcvm_t *qcvm, int global)
{
	return GET_ENTITY(global) / sizeof(qcvm_entity_t);
}

/* retrieve global int */
int qcvm_get_global_int(qcvm_t *qcvm, int global)
{
	return GET_INT(global);
}

/* retrieve global float */
float qcvm_get_global_float(qcvm_t *qcvm, int global)
{
	return GET_FLOAT(global);
}

/* retrieve global vector */
qcvm_vec3 qcvm_get_global_vector(qcvm_t *qcvm, int global)
{
	qcvm_vec3 ret;

	ret.x = GET_FLOAT(global);
	ret.y = GET_FLOAT(global + 1);
	ret.z = GET_FLOAT(global + 2);

	return ret;
}

/* find global by name */
int qcvm_find_global(qcvm_t *qcvm, const char *name)
{
	/* variables */
	int i;

	/* search globals */
	for (i = 1; i < qcvm->header->num_global_vars; i++)
	{
		if (strcmp(GET_STRING_OFS(qcvm->global_vars[i].name), name) == 0)
			return qcvm->global_vars[i].ofs;
	}

	/* return failure */
	return -1;
}

/*
 * set parameters
 */

/* set string parameter */
void qcvm_set_parm_string(qcvm_t *qcvm, int parm, const char *s)
{
	/* bounds check */
	if (qcvm->tempstrings_ptr + strlen(s) + 1 > qcvm->tempstrings + TEMPSTRINGS_SIZE)
		qcvm->tempstrings_ptr = qcvm->tempstrings;

	/* sprintf */
	sprintf(qcvm->tempstrings_ptr, "%s", s);

	/* return str */
	GET_INT(OFS_PARM0 + (parm * 3)) = (qcvm->tempstrings_ptr) - qcvm->strings;

	/* advance ptr */
	qcvm->tempstrings_ptr += strlen(s) + 1;
}

/* set vector parameter */
void qcvm_set_parm_vector(qcvm_t *qcvm, int parm, float a, float b, float c)
{
	GET_FLOAT(OFS_PARM0 + (parm * 3)) = a;
	GET_FLOAT(OFS_PARM0 + (parm * 3) + 1) = b;
	GET_FLOAT(OFS_PARM0 + (parm * 3) + 2) = c;
}

/* set integer parameter */
void qcvm_set_parm_int(qcvm_t *qcvm, int parm, int val)
{
	GET_INT(OFS_PARM0 + (parm * 3)) = val;
}

/* set float parameter */
void qcvm_set_parm_float(qcvm_t *qcvm, int parm, float val)
{
	GET_FLOAT(OFS_PARM0 + (parm * 3)) = val;
}

/*
 * get parameters
 */

/* get argc */
int qcvm_get_argc(qcvm_t *qcvm)
{
	return qcvm->function_argc;
}

/* get entity parameter */
int qcvm_get_parm_entity(qcvm_t *qcvm, int parm)
{
	return GET_ENTITY(OFS_PARM0 + (parm * 3)) / sizeof(qcvm_entity_t);
}

/* get vector parameter */
qcvm_vec3 qcvm_get_parm_vector(qcvm_t *qcvm, int parm)
{
	qcvm_vec3 ret;

	ret.x = GET_FLOAT(OFS_PARM0 + (parm * 3));
	ret.y = GET_FLOAT(OFS_PARM0 + (parm * 3) + 1);
	ret.z = GET_FLOAT(OFS_PARM0 + (parm * 3) + 2);

	return ret;
}

/* get string parameter */
const char *qcvm_get_parm_string(qcvm_t *qcvm, int parm)
{
	return GET_STRING(OFS_PARM0 + (parm * 3));
}

/* get integer parameter */
int qcvm_get_parm_int(qcvm_t *qcvm, int parm)
{
	return GET_INT(OFS_PARM0 + (parm * 3));
}

/* get float parameter */
float qcvm_get_parm_float(qcvm_t *qcvm, int parm)
{
	return GET_FLOAT(OFS_PARM0 + (parm * 3));
}

/*
 * return values 
 */

/* return a new entity */
void qcvm_return_entity(qcvm_t *qcvm, int entity)
{
	RETURN_ENTITY(&qcvm->entities[entity]);
}

/* return a string to the previous function */
void qcvm_return_string(qcvm_t *qcvm, const char *s)
{
	/* bounds check */
	if (qcvm->tempstrings_ptr + strlen(s) + 1 > qcvm->tempstrings + TEMPSTRINGS_SIZE)
		qcvm->tempstrings_ptr = qcvm->tempstrings;

	/* sprintf */
	sprintf(qcvm->tempstrings_ptr, "%s", s);

	/* return str */
	RETURN_STRING(qcvm->tempstrings_ptr);

	/* advance ptr */
	qcvm->tempstrings_ptr += strlen(s) + 1;
}

/* return a vector to the previous function */
void qcvm_return_vector(qcvm_t *qcvm, float a, float b, float c)
{
	RETURN_VECTOR(a, b, c);
}

/* return a int to the previous function */
void qcvm_return_int(qcvm_t *qcvm, int val)
{
	RETURN_INT(val);
}

/* return a float to the previous function */
void qcvm_return_float(qcvm_t *qcvm, float val)
{
	RETURN_FLOAT(val);
}

/*
 * exports
 */

/* add export */
void qcvm_add_export(qcvm_t *qcvm, qcvm_export_t export)
{
	qcvm->exports[qcvm->num_exports] = export;
	qcvm->num_exports++;
}

/*
 * misc
 */

/* get function by name search */
int qcvm_find_function(qcvm_t *qcvm, const char *name)
{
	/* variables */
	int i;

	/* loop through functions */
	for (i = 1; i < qcvm->header->num_functions; i++)
	{
		if (strcmp(name, GET_STRING_OFS(qcvm->functions[i].name)) == 0)
			return i;
	}

	/* return failure */
	return -1;
}

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

#ifndef _QCVM_H_
#define _QCVM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* config header */
#include "qcvmconf.h"

/* result codes */
enum {
	QCVM_OK,
	QCVM_UNKNOWN_ERROR,
	QCVM_NULL_POINTER,
	QCVM_FUNCTION_NOT_FOUND,
	QCVM_INVALID_OPCODE,
	QCVM_INVALID_FUNCTION,
	QCVM_INVALID_RESULT_CODE,
	QCVM_BUILTIN_CALL,
	QCVM_STATE_CALL,
	QCVM_BUILTIN_NOT_FOUND,
	QCVM_EXECUTION_FINISHED,
	QCVM_INVALID_PROGS,
	QCVM_UNSUPPORTED_VERSION,
	QCVM_UNSUPPORTED_OPCODE,
	QCVM_UNSUPPORTED_FUNCTION,
	QCVM_STACK_OVERFLOW,
	QCVM_STACK_UNDERFLOW,
	QCVM_ARGUMENT_OUT_OF_RANGE,
	QCVM_NO_TEMPSTRINGS,
	QCVM_NO_ENTITIES,
	QCVM_NUM_RESULT_CODES
};

/* qcvm variable types */
enum {
	QCVM_TYPE_VOID,
	QCVM_TYPE_STRING,
	QCVM_TYPE_FLOAT,
	QCVM_TYPE_VECTOR,
	QCVM_TYPE_ENTITY,
	QCVM_TYPE_FIELD,
	QCVM_TYPE_FUNCTION,
	QCVM_TYPE_POINTER
};

/* main container */
typedef struct qcvm {

	/** progs file buffer and size
	 *
	 * this should be loaded directly from the disk into this buffer. there's
	 * not really any way to generate these programmatically besides writing
	 * your own qc compiler. qcvm expects the entire file structure to be
	 * present.
	 */
	size_t len_progs;
	void *progs;

	/** entities buffer
	 *
	 * allocate this to a suitably large size to store entity definitions and
	 * all their fields.
	 */
	size_t len_entities;
	void *entities;

	/** state handler callback function
	 *
	 * when OPCODE_STATE is called, it should update the current "self"
	 * entity's animation frame and do something with the next function in the
	 * chain. this must be handled in a program-agnostic sort of way, so
	 * there's a callback function for the user to handle it.
	 */
	int (*state_callback)(struct qcvm *qcvm, float frame, int function, void *user);
	void *state_callback_user;

	/** native functions
	 *
	 * these functions are called from qc if the call opcode is <=0. first,
	 * qcvm will see if the opcode number (made positive) is a valid index into
	 * the array. if not, it will search the array for one with a name string
	 * matching the function called from qc. NOTE: QCVM will update the opcode
	 * in memory to point to the correct builtin array index. this will make
	 * for slightly faster execution in further steps.
	 */
	size_t num_builtins;
	struct qcvm_builtin {
		const char *name;
		int (*func)(struct qcvm *qcvm, void *user);
		void *user;
	} *builtins;

	/** tempstring store
	 *
	 * qc relies heavily on tempstrings. the original qcvm only had a single
	 * 128 byte buffer for tempstrings returned one at a time. this meant that
	 * tempstrings would immediately expire as soon as they were overwritten,
	 * with no way to tell from qc. elsewhere, it would return a 32-bit offset
	 * from the existing string table to wherever the string was in general
	 * memory. later quake engines have added enhancements like garbage
	 * collection and reference counting.
	 *
	 * qcvm takes the current tempstrings buffer position and writes the new
	 * strings after it, wrapping around to the start if it's reached the end
	 * of the buffer.
	 *
	 * if this buffer is null, qcvm will not be able to return strings from
	 * builtins.
	 */
	size_t len_tempstrings;
	char *tempstrings;

	/*
	 *
	 * "private" fields, don't mess with these.
	 *
	 */

	/* file header */
	struct qcvm_header {
		uint32_t version;
		uint32_t crc;
		uint32_t ofs_statements;
		uint32_t num_statements;
		uint32_t ofs_global_vars;
		uint32_t num_global_vars;
		uint32_t ofs_field_vars;
		uint32_t num_field_vars;
		uint32_t ofs_functions;
		uint32_t num_functions;
		uint32_t ofs_strings;
		uint32_t len_strings;
		uint32_t ofs_globals;
		uint32_t num_globals;
		uint32_t num_entity_fields;
	} header;

	/* statements */
	size_t num_statements;
	struct qcvm_statement {
		uint16_t opcode;
		int16_t vars[3];
	} *statements;

	/* functions */
	size_t num_functions;
	struct qcvm_function {
		int32_t first_statement;
		int32_t first_parm;
		int32_t num_locals;
		int32_t profile;
		int32_t ofs_name;
		int32_t ofs_filename;
		int32_t num_parms;
		uint8_t parm_sizes[8];
	} *functions;

	/* strings */
	size_t len_strings;
	char *strings;

	/* field vars */
	size_t num_field_vars;
	struct qcvm_var {
		uint16_t type;
		uint16_t ofs;
		int32_t name;
	} *field_vars;

	/* global vars */
	size_t num_global_vars;
	struct qcvm_var *global_vars;

	/* globals */
	size_t num_globals;
	union qcvm_global {
		float f;
		int32_t i;
		uint32_t ui;
	} *globals;

	/*
	 *
	 * runtime stuff.
	 *
	 */

	/* stack */
	struct qcvm_stack {
		int32_t statement;
		struct qcvm_function *function;
	} stack[QCVM_STACK_DEPTH];
	int32_t stack_depth;
	int32_t local_stack[QCVM_LOCAL_STACK_DEPTH];
	int32_t local_stack_used;
	struct qcvm_stack xstack;

	/* execution state */
	struct qcvm_function *current_function;
	struct qcvm_function *next_function;
	struct qcvm_statement *current_statement;
	int32_t current_statement_index;
	int32_t current_builtin;
	int32_t exit_depth;
	int32_t current_argc;
	char *tempstrings_ptr;

	/* function evaluation */
	union qcvm_eval {
		int32_t s;
		float f;
		float v[3];
		int32_t func;
		int32_t field;
		int32_t i;
		uint32_t e;
	} *eval[3];

} qcvm_t;

/**
 * \brief initialize qcvm structure
 *
 * this function parses the progs buffer and sets up pointers to the various
 * structures present in the file. it also checks for what features are
 * available, such as entities and tempstrings.
 *
 * \param qcvm virtual machine to init
 * \returns result code
 */
int qcvm_init(qcvm_t *qcvm);

/**
 * \brief query qcvm for amount of memory needed for per-entity storage
 *
 * qcvm requires a writeable buffer to store entities and their fields. each
 * entity field is 12 bytes, and the amount of entity fields in a progs file is
 * user-defined. use this function to allocate a suitable amount of space.
 *
 * this function may be called before qcvm_init().
 *
 * usage example:
 *
 * const int max_entities = 8192;
 * size_t entity_fields = 0;
 * size_t entity_size = 0;
 * qcvm_query_entity_info(&qcvm, &entity_fields, &entity_size);
 * qcvm.entities = malloc(max_entities * entity_size);
 *
 * \param qcvm virtual machine to query
 * \param num_fields pointer to size_t to contain the number of fields
 * \param size pointer to size_t to contain the size of an entity
 * \returns result code
 */
int qcvm_query_entity_info(qcvm_t *qcvm, size_t *num_fields, size_t *size);

/**
 * \brief get static string from result code
 * \param r result code
 * \returns static null terminated result string
 */
const char *qcvm_result_string(int r);

/**
 * \brief queue up function for execution
 * \param qcvm virtual machine to use
 * \param name function name
 * \returns result code
 */
int qcvm_load(qcvm_t *qcvm, const char *name);

/**
 * \brief execute one qcvm step
 * \param qcvm virtual machine to use
 * \returns result code
 */
int qcvm_step(qcvm_t *qcvm);

/**
 * \brief queue up and execute the named function
 * \param qcvm virtual machine to use
 * \param name case-sensitive function name
 * \returns result code
 */
int qcvm_run(qcvm_t *qcvm, const char *name);

/**
 * \brief return a string to the function that called this one
 * \param qcvm virtual machine to use
 * \param s null-terminated string
 * \returns result code
 */
int qcvm_return_string(qcvm_t *qcvm, const char *s);

/**
 * \brief return a float to the function that called this one
 * \param qcvm virtual machine to use
 * \param f float value
 * \returns result code
 */
int qcvm_return_float(qcvm_t *qcvm, float f);

/**
 * \brief return a vector to the function that called this one
 * \param qcvm virtual machine to use
 * \param a vector x value
 * \param b vector y value
 * \param c vector z value
 * \returns result code
 */
int qcvm_return_vector(qcvm_t *qcvm, float x, float y, float z);

/**
 * \brief return an entity index to the function that called this one
 * \param qcvm virtual machine to use
 * \param e entity index
 * \returns result code
 */
int qcvm_return_entity(qcvm_t *qcvm, uint32_t e);

/**
 * \brief query argument count of current function
 * \param qcvm virtual machine to use
 * \param argc integer to fill
 * \returns result code
 */
int qcvm_query_argument_count(qcvm_t *qcvm, int *argc);

/**
 * \brief retrieve function return value as a float
 * \param qcvm virtual machine to use
 * \param f float to fill
 * \returns result code
 */
int qcvm_get_return_float(qcvm_t *qcvm, float *f);

/**
 * \brief retrieve function return value as a vector
 * \param qcvm virtual machine to use
 * \param x float to fill
 * \param y float to fill
 * \param z float to fill
 * \returns result code
 */
int qcvm_get_return_vector(qcvm_t *qcvm, float *x, float *y, float *z);

/**
 * \brief retrieve function return value as an int
 * \param qcvm virtual machine to use
 * \param i int to fill
 * \returns result code
 */
int qcvm_get_return_int(qcvm_t *qcvm, int *i);

/**
 * \brief retrieve function return value as an entity index
 * \param qcvm virtual machine to use
 * \param e unsigned integer to fill
 * \returns result code
 */
int qcvm_get_return_entity(qcvm_t *qcvm, uint32_t *e);

/**
 * \brief retrieve string from function argument
 * \param qcvm virtual machine to use
 * \param i argument index
 * \param s string pointer to fill
 * \returns result code
 */
int qcvm_get_argument_string(qcvm_t *qcvm, int i, const char **s);

/**
 * \brief retrieve float from function argument
 * \param qcvm virtual machine to use
 * \param i argument index
 * \param f float to fill
 * \returns result code
 */
int qcvm_get_argument_float(qcvm_t *qcvm, int i, float *f);

/**
 * \brief retrieve int from function argument
 * \param qcvm virtual machine to use
 * \param i argument index
 * \param i int to fill
 * \returns result code
 */
int qcvm_get_argument_int(qcvm_t *qcvm, int i, int *n);

/**
 * \brief retrieve vector from function argument
 * \param qcvm virtual machine to use
 * \param i argument index
 * \param x float to fill
 * \param y float to fill
 * \param z float to fill
 * \returns result code
 */
int qcvm_get_argument_vector(qcvm_t *qcvm, int i, float *x, float *y, float *z);

/**
 * \brief retrieve entity index from function argument
 * \param qcvm virtual machine to use
 * \param i argument index
 * \param e unsigned integer to fill
 * \returns result code
 */
int qcvm_get_argument_entity(qcvm_t *qcvm, int i, uint32_t *e);

#ifdef __cplusplus
}
#endif
#endif /* _QCVM_H_ */

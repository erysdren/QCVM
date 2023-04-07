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
#ifndef _EQCC_PRIVATE_H_
#define _EQCC_PRIVATE_H_
#ifdef __cplusplus
extern "C" {
#endif

/*
 * headers
 */

/* qcvm */
#include "qcvm_private.h"

/* stb */
#include "stb_c_lexer.h"

/* eqcc */
#include "eqcc.h"

/*
 * macros
 */

/* max values */
#define	MAX_STRINGS 0x10000
#define	MAX_GLOBALS 0x4000
#define	MAX_FIELDS 0x400
#define	MAX_STATEMENTS 0x10000
#define	MAX_FUNCTIONS 0x2000
#define MAX_TOKENS 0x10000
#define MAX_STORE 0x40

/*
 * prototypes
 */

/* eqcc_compile.c */
extern int tokens[MAX_TOKENS];
extern int num_tokens;
extern char strings[MAX_STRINGS];
extern int ofs_strings;
extern qcvm_statement_t statements[MAX_STATEMENTS];
extern int num_statemnts;
extern qcvm_function_t functions[MAX_FUNCTIONS];
extern int num_functions;

/* eqcc_utils.c */
void *load_file(const char *filename, int *buffer_len);
void error(const char *fmt, ...);
void print_help_text();

/* guard */
#ifdef __cplusplus
}
#endif
#endif /* _EQCC_PRIVATE_H_ */

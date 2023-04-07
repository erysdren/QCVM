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
#include <stdarg.h>
#include <string.h>

/* qcvm */
#include "qcvm_private.h"

/* eqcc */
#include "eqcc_private.h"

/* stb */
#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

/*
 * globals
 */

/* tokens */
int tokens[MAX_TOKENS];
int num_tokens;

/* strings */
char strings[MAX_STRINGS];
int ofs_strings;

/* statements */
qcvm_statement_t statements[MAX_STATEMENTS];
int num_statemnts;

/* functions */
qcvm_function_t functions[MAX_FUNCTIONS];
int num_functions;

/*
 * functions
 */

/* print lex tokens */
static void print_token(stb_lexer *lexer)
{
	switch (lexer->token)
	{
		/* valid lex tokens */
		case CLEX_id: printf("_%s", lexer->string); break;
		case CLEX_eq: printf("=="); break;
		case CLEX_noteq: printf("!="); break;
		case CLEX_lesseq: printf("<="); break;
		case CLEX_greatereq: printf(">="); break;
		case CLEX_andand: printf("&&"); break;
		case CLEX_oror: printf("||"); break;
		case CLEX_shl: printf("<<"); break;
		case CLEX_shr: printf(">>"); break;
		case CLEX_plusplus: printf("++"); break;
		case CLEX_minusminus: printf("--"); break;
		case CLEX_arrow: printf("->"); break;
		case CLEX_andeq: printf("&="); break;
		case CLEX_oreq: printf("|="); break;
		case CLEX_xoreq: printf("^="); break;
		case CLEX_pluseq: printf("+="); break;
		case CLEX_minuseq: printf("-="); break;
		case CLEX_muleq: printf("*="); break;
		case CLEX_diveq: printf("/="); break;
		case CLEX_modeq: printf("%%="); break;
		case CLEX_shleq: printf("<<="); break;
		case CLEX_shreq: printf(">>="); break;
		case CLEX_eqarrow: printf("=>"); break;
		case CLEX_dqstring: printf("\"%s\"", lexer->string); break;
		case CLEX_sqstring: printf("'\"%s\"'", lexer->string); break;
		case CLEX_charlit: printf("'%s'", lexer->string); break;
		case CLEX_intlit: printf("#%ld", lexer->int_number); break;
		case CLEX_floatlit: printf("%g", lexer->real_number); break;

		/* invalid token */
		default:
		{
			if (lexer->token >= 0 && lexer->token < 256)
				printf("%c", (int)lexer->token);
			else
				printf("<<<UNKNOWN TOKEN %ld >>>\n", lexer->token);

			break;
		}
	}
}

/* compile qc file */
int eqcc_compile(const char *filename)
{
	/* variables */
	void *buffer;
	int buffer_len;
	char *buffer_start;
	char *buffer_end;
	stb_lexer lex;
	char string_store[128];

	/* load file */
	buffer = load_file(filename, &buffer_len);
	if (!buffer) return 0;

	/* init lexer */
	buffer_start = (char *)buffer;
	buffer_end = buffer_start + buffer_len;
	stb_c_lexer_init(&lex, buffer_start, buffer_end, string_store, 128);

	/* do lex */
	while (stb_c_lexer_get_token(&lex))
	{
		print_token(&lex);
	}

	printf("\n");

	/* free memory */
	free(buffer);

	/* return success */
	return 1;
}

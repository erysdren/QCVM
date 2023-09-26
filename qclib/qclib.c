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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* qcvm */
#include "qcvm.h"

/* qclib */
#include "qclib.h"

/* print text */
void qclib_print(qcvm_t *qcvm)
{
	int i;

	for (i = 0; i < qcvm_get_argc(qcvm); i++)
	{
		fprintf(stdout, "%s", qcvm_get_parm_string(qcvm, i));
	}

	fflush(stdout);
}

qcvm_export_t export_print =
{
	.func = qclib_print,
	.name = "print",
	.type = QCVM_VOID,
	.argc = 2,
	.args[0] = {.name = "s", .type = QCVM_STRING},
	.args[1] = {.name = "...", .type = QCVM_VARGS}
};

/* print formatted text */
void qclib_printf(qcvm_t *qcvm)
{
	qcvm_vec3_t v;
	const char *fmt;
	char c;
	int arg;

	/* get fmt string */
	fmt = qcvm_get_parm_string(qcvm, 0);

	/* loups */
	arg = 1;
	while ((c = *fmt++))
	{
		/* sanity check */
		if (arg > qcvm_get_argc(qcvm))
			break;

		/* not a format char */
		if (c != '%')
		{
			fputc(c, stdout);
			continue;
		}

		/* do formatting */
		switch ((c = *fmt++))
		{
			/* % */
			case '%':
				fputc(c, stdout);
				break;

			/* string */
			case 's':
				fprintf(stdout, "%s", qcvm_get_parm_string(qcvm, arg));
				arg++;
				break;

			/* int */
			case 'd':
				fprintf(stdout, "%d", qcvm_get_parm_int(qcvm, arg));
				arg++;
				break;

			/* float */
			case 'f':
				fprintf(stdout, "%0.4f", qcvm_get_parm_float(qcvm, arg));
				arg++;
				break;

			/* vector */
			case 'v':
				v = qcvm_get_parm_vector(qcvm, arg);
				fprintf(stdout, "%0.4f %0.4f %0.4f", v.x, v.y, v.z);
				arg++;
				break;
		}
	}

	/* print to stdout */
	fflush(stdout);
}

qcvm_export_t export_printf =
{
	.func = qclib_printf,
	.name = "printf",
	.type = QCVM_VOID,
	.argc = 2,
	.args[0] = {.name = "fmt", .type = QCVM_STRING},
	.args[1] = {.name = "...", .type = QCVM_VARGS}
};

/* print formatted text and return tempstring */
void qclib_sprintf(qcvm_t *qcvm)
{
	qcvm_vec3_t v;
	const char *fmt;
	char c;
	int arg;
	static char buffer[1024];

	/* get fmt string */
	fmt = qcvm_get_parm_string(qcvm, 0);

	/* loups */
	arg = 1;
	while ((c = *fmt++))
	{
		/* sanity check */
		if (arg > qcvm_get_argc(qcvm))
			break;

		/* not a format char */
		if (c != '%')
		{
			sprintf(buffer, "%c", c);
			continue;
		}

		/* do formatting */
		switch ((c = *fmt++))
		{
			/* % */
			case '%':
				sprintf(buffer, "%c", c);
				break;

			/* string */
			case 's':
				sprintf(buffer, "%s", qcvm_get_parm_string(qcvm, arg));
				arg++;
				break;

			/* int */
			case 'd':
				sprintf(buffer, "%d", qcvm_get_parm_int(qcvm, arg));
				arg++;
				break;

			/* float */
			case 'f':
				sprintf(buffer, "%0.4f", qcvm_get_parm_float(qcvm, arg));
				arg++;
				break;

			/* vector */
			case 'v':
				v = qcvm_get_parm_vector(qcvm, arg);
				sprintf(buffer, "%0.4f %0.4f %0.4f", v.x, v.y, v.z);
				arg++;
				break;
		}
	}

	/* return the string */
	qcvm_return_string(qcvm, buffer);
}

qcvm_export_t export_sprintf =
{
	.func = qclib_sprintf,
	.name = "sprintf",
	.type = QCVM_STRING,
	.argc = 2,
	.args[0] = {.name = "fmt", .type = QCVM_STRING},
	.args[1] = {.name = "...", .type = QCVM_VARGS}
};

/* spawn entity */
void qclib_spawn(qcvm_t *qcvm)
{
	qcvm_return_entity(qcvm, qcvm_add_entity(qcvm));
}

qcvm_export_t export_spawn =
{
	.func = qclib_spawn,
	.name = "spawn",
	.type = QCVM_ENTITY,
	.argc = 0
};

/* get length of string */
void qclib_strlen(qcvm_t *qcvm)
{
	qcvm_return_float(qcvm, (float)strlen(qcvm_get_parm_string(qcvm, 0)));
}

qcvm_export_t export_strlen =
{
	.func = qclib_strlen,
	.name = "strlen",
	.type = QCVM_FLOAT,
	.argc = 1,
	.args[0] = {.name = "s", .type = QCVM_STRING}
};

/* return a concat of two strings */
void qclib_strcat(qcvm_t *qcvm)
{
	/* variables */
	const char *s;
	int slen;
	int pos;
	char buf[512];
	char *ptr;
	int i;

	ptr = buf;

	/* print strings */
	pos = 0;
	for (i = 0; i < qcvm_get_argc(qcvm); i++)
	{
		/* get string parm */
		s = qcvm_get_parm_string(qcvm, i);
		slen = strlen(s);
		pos += slen;

		/* sprintf it */
		snprintf(ptr, sizeof(buf) - pos, "%s", s);
		ptr += slen;
	}

	qcvm_return_string(qcvm, buf);
}

qcvm_export_t export_strcat =
{
	.func = qclib_strcat,
	.name = "strcat",
	.type = QCVM_STRING,
	.argc = 3,
	.args[0] = {.name = "s1", .type = QCVM_STRING},
	.args[1] = {.name = "s2", .type = QCVM_STRING},
	.args[2] = {.name = "...", .type = QCVM_VARGS}
};

/* float to string */
void qclib_ftos(qcvm_t *qcvm)
{
	/* variables */
	float f;
	char buf[64];

	/* get parms */
	f = qcvm_get_parm_float(qcvm, 0);

	/* sprintf */
	if (f == (int)f)
		sprintf(buf, "%d", (int)f);
	else
		sprintf(buf, "%0.4f", f);

	/* return */
	qcvm_return_string(qcvm, buf);
}

qcvm_export_t export_ftos =
{
	.func = qclib_ftos,
	.name = "ftos",
	.type = QCVM_STRING,
	.argc = 1,
	.args[0] = {.name = "f", .type = QCVM_FLOAT}
};

/* vector to string */
void qclib_vtos(qcvm_t *qcvm)
{
	/* variables */
	qcvm_vec3_t v;
	char buf[64];

	/* get parms */
	v = qcvm_get_parm_vector(qcvm, 0);

	/* sprintf */
	sprintf(buf, "%0.4f %0.4f %0.4f", v.x, v.y, v.z);

	/* return */
	qcvm_return_string(qcvm, buf);
}

qcvm_export_t export_vtos =
{
	.func = qclib_vtos,
	.name = "vtos",
	.type = QCVM_STRING,
	.argc = 1,
	.args[0] = {.name = "v", .type = QCVM_VECTOR}
};

/* get substring */
void qclib_substring(qcvm_t *qcvm)
{
	/* variables */
	char buf[512];
	int start;
	int len;
	const char *str;

	/* get parms */
	str = qcvm_get_parm_string(qcvm, 0);
	start = (int)qcvm_get_parm_float(qcvm, 1);
	len = (int)qcvm_get_parm_float(qcvm, 2);

	/* copy string */
	memcpy(buf, str + start, len);

	/* return */
	qcvm_return_string(qcvm, buf);
}

qcvm_export_t export_substring =
{
	.func = qclib_substring,
	.name = "substring",
	.type = QCVM_STRING,
	.argc = 3,
	.args[0] = {.name = "s", .type = QCVM_STRING},
	.args[1] = {.name = "start", .type = QCVM_FLOAT},
	.args[2] = {.name = "len", .type = QCVM_FLOAT}
};

/* install qclib default builtin functions */
void qclib_install(qcvm_t *qcvm)
{
	qcvm_add_export(qcvm, &export_print);
	qcvm_add_export(qcvm, &export_printf);
	qcvm_add_export(qcvm, &export_sprintf);
	qcvm_add_export(qcvm, &export_spawn);
	qcvm_add_export(qcvm, &export_strlen);
	qcvm_add_export(qcvm, &export_strcat);
	qcvm_add_export(qcvm, &export_substring);
	qcvm_add_export(qcvm, &export_vtos);
	qcvm_add_export(qcvm, &export_ftos);
}

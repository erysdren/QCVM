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
 * File:			/source/test.c
 *
 * Description:		QCVM test application
 *
 * Last Modified:	February 6th 2023
 *
 *****************************************************************************/

/*
 * headers
 */

/* std */
#include <stdio.h>
#include <stdlib.h>

/* qcvm */
#include "qcvm.h"

/*
 * builtin functions
 */

/* print */
void print(void)
{
	int i;

	for (i = 0; i < qc_argc; i++)
	{
		printf("%s", QC_GET_STRING(QC_OFS_PARM0 + i * 3));
	}
}

/* float to string */
void ftos(void)
{
	/* variables */
	float f;

	/* assign float */
	f = QC_GET_FLOAT(QC_OFS_PARM0);

	/* sprintf it */
	sprintf(qc_string_temp, "%1.2f", f);

	/* assign return val */
	QC_GET_INT(QC_OFS_RETURN) = qc_string_temp - qc_strings;
}

/*
 * main
 */

/* main */
int main(int argc, char **argv)
{
	/* load qc */
	qc_load("../qc/progs.dat");

	/* load builtins */
	qc_builtin_add(print);
	qc_builtin_add(ftos);

	/* call init() function */
	qc_execute(qc_function_get("init"));

	/* call update() function */
	qc_execute(qc_function_get("update"));

	/* call input_test() function */
	qc_set_parm_float(0, 0.5f);
	qc_execute(qc_function_get("input_test"));

	/* exit */
	qc_exit();

	/* return success */
	return EXIT_SUCCESS;
}

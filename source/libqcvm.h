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
 * File:			/source/libqcvm.h
 *
 * Description:		QCVM lib header
 *
 * Last Modified:	February 5th 2023
 *
 *****************************************************************************/

/* globals */
extern qc_builtin_t qcvm_builtins[];
extern int qcvm_num_builtins;

/*
 * functions
 */

void qcvm_null(void);
void qcvm_print(void);

![QCVM Logo](./.github/logo2.png "QCVM")

A bite-sized QuakeC virtual machine written in C, based on the original Quake Engine
source code release from 1999.

## Resources

- [State-of-the-art QuakeC compiler.](https://www.fteqcc.org/)
- [FTEQCC reference manual.](https://icculus.org/~marco/quakec/fteqcc_manual.txt)
- [Kaitai Struct definitions file for the QuakeC "progs" format.](./tools/progs.ksy)
- [QuakeC highlighting for VS Code by VictorF.](https://marketplace.visualstudio.com/items?itemName=VictorF.quakec)
- [QuakeC highlighting for VS Code by joshuaskelly.](https://marketplace.visualstudio.com/items?itemName=joshuaskelly.quakec)
- [InsideQC tutorials.](https://www.insideqc.com/)
- [QuakeC tutorials at quakewiki.org.](https://quakewiki.org/wiki/QuakeC_tutorials)
- [QuakeC Reference Manual.](https://pages.cs.wisc.edu/~jeremyp/quake/quakec/quakec.pdf)

## Usage

See the [Example Implementation](./test/test.c) for more inforamtion.

```c
#include "qcvm.h"

void example()
{
	float f = QC_GET_FLOAT(QC_OFS_PARM0);
	f += 2;
	QC_RETURN_FLOAT(f);
}

qc_export_t export_example = {
	.name = "example",
	.desc = "returns float += 2",
	.func = example,
	.ret = QC_TYPE_FLOAT,
	.parmc = 1,
	.parms[0] = {"f", QC_TYPE_FLOAT}
};

int main()
{
	/* load compiled qc */
	qc_load("progs.dat");

	/* add export */
	qc_add_export(&export_example);

	/* dump export to valid qc */
	qc_dump_exports("exports.qc");

	/* execute qc function by name */
	qc_execute(qc_function_get("init"));

	/* destroy qc context */
	qc_exit();

	return 0;
}
```

## Contact

- [Discord](https://discord.gg/5MwE3xMcdN)
- [Email](mailto:jaycie@erysdren.me)

## Todo

- Refactor code to remove globals, allowing for multiple instances to run simultainiously.
- Add function-search to call specific functions by name, along with setting function parameters.
- Easy API for adding builtin functions.
- Dump user-added builtin functions to properly formatted QC file.
- Improve tempstring handling.
- Add support for HexenC and FTEQCC's extended opcodes.
- Add support for the QTest progs format.
- Bindings for other languages.

## License

GNU GENERAL PUBLIC LICENSE

QCVM is Copyright (C) 2023-2024 erysdren (it/she)

Quake is Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

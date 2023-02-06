# QCVM

A bite-sized QuakeC virtual machine written in C, based on the original Quake Engine
source code release from 1999.

## Tools

- [State-of-the-art QuakeC compiler](https://www.fteqcc.org/)
- [Kaitai Struct definitions file for the QuakeC "progs" format.](./tools/progs.ksy)

## Todo

- Refactor code to remove globals, allowing for multiple instances to run simultainiously.
- Add function-search to call specific functions by name, along with setting function parameters.
- Easy API for adding builtin functions.
- Dump user-added builtin functions to properly formatted QC file.
- Improve tempstring handling.
- Add support for HexenC and FTEQCC's extended opcodes.
- Add support for the QTest progs format.

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

![QCVM Logo](./.github/logo2.png "QCVM")

A bite-sized QuakeC virtual machine written in C.

This repository contains various tools and libraries:

- `qcvm`: The main QuakeC virtual machine library.
- `qclib`: An additional library of useful functions for C to QuakeC interop.
- `qcpkg`: A command-line tool that can produce native executables from QuakeC modules.

It also includes some examples:

- `examples/module_test`: Test application for communicating between two different QuakeC modules.
- `examples/qcpong`: PONG clone made with SDL2 and QCVM.
- `examples/qcvm_test`: The main basic test application.
- `examples/qcvm_test_fte`: Test application for the extended QuakeC opcodes provided by FTE Quake.

A new, experimental version of the QCVM can be found in the `new` directory of
the source tree.

## Resources

- [State-of-the-art QuakeC compiler](https://www.fteqcc.org/)
- [FTEQCC reference manual](https://icculus.org/~marco/quakec/fteqcc_manual.txt)
- [Kaitai Struct definitions file for the QuakeC "progs" format](./contrib/progs.ksy)
- [QuakeC highlighting for VS Code by VictorF](https://marketplace.visualstudio.com/items?itemName=VictorF.quakec)
- [QuakeC highlighting for VS Code by joshuaskelly](https://marketplace.visualstudio.com/items?itemName=joshuaskelly.quakec)
- [QuakeC tutorials at insideqc.com](https://www.insideqc.com/)
- [QuakeC tutorials at quakewiki.org](https://quakewiki.org/wiki/QuakeC_tutorials)
- [QuakeC Reference Manual (1998)](./contrib/quakec-reference-manual.pdf)
- [QuakeC MIME specification](./contrib/x-quakec.xml)

## Usage

- [USAGE.md](./USAGE.md)

## Building

QCVM is built via CMake.

Linux:
```
mkdir cmake-build && cd cmake-build
cmake ..
make
```

## Feedback

- [Hacker News (February 6th, 2023)](https://news.ycombinator.com/item?id=34672598)

## License

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

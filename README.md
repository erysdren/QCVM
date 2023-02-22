![QCVM Logo](./.github/logo2.png "QCVM")

A bite-sized QuakeC virtual machine written in C.

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

- [USAGE.md](./USAGE.md)

## Bindings

- [FreePascal](./bindings/pascal/qcvm.pas)

## Contact

- [Discord](https://discord.gg/5MwE3xMcdN)
- [Email](mailto:jaycie@erysdren.me)

## Todo

- [ ] Dump user-added builtin functions to properly formatted QC file.
- [ ] Add support for HexenC and FTEQCC's extended opcodes.
- [ ] Add support for the QTest progs format.
- [ ] Bindings for other languages.
- [ ] Add formatted string return function.
- [ ] Split up `qcvm_utils.c` into seperate files for easier browsing and modification.
- [ ] Keep USAGE.MD updated.

## License

MIT License

Copyright (c) 2023 erysdren (it/she)

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

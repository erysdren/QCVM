meta:
  id: progs
  file-extension: dat
  endian: le
  bit-endian: le

doc-ref: |
  https://github.com/id-Software/Quake/blob/master/WinQuake/pr_comp.h

doc: |
  Quake's "Programs" format. Built on opcodes and runs inside a virtual machine.

seq:
  - id: header
    type: progs_header_t

instances:

  strings:
    pos: header.ofs_strings
    size: header.num_strings

  functions:
    pos: header.ofs_functions
    type: progs_function_t
    repeat: expr
    repeat-expr: header.num_functions

  fielddefs:
    pos: header.ofs_fielddefs
    type: progs_def_t
    repeat: expr
    repeat-expr: header.num_fielddefs

  globaldefs:
    pos: header.ofs_globaldefs
    type: progs_def_t
    repeat: expr
    repeat-expr: header.num_globaldefs

  statements:
    pos: header.ofs_statements
    type: progs_statement_t
    repeat: expr
    repeat-expr: header.num_statements

  globals:
    pos: header.ofs_globals
    type: progs_global_t
    repeat: expr
    repeat-expr: header.num_globals

types:

  progs_header_t:
    seq:
      - id: version
        type: s4
      - id: crc
        type: s4
      - id: ofs_statements
        type: s4
      - id: num_statements
        type: s4
      - id: ofs_globaldefs
        type: s4
      - id: num_globaldefs
        type: s4
      - id: ofs_fielddefs
        type: s4
      - id: num_fielddefs
        type: s4
      - id: ofs_functions
        type: s4
      - id: num_functions
        type: s4
      - id: ofs_strings
        type: s4
      - id: num_strings
        type: s4
      - id: ofs_globals
        type: s4
      - id: num_globals
        type: s4
      - id: entityfields
        type: s4

  progs_function_t:
    seq:
      - id: first_statement
        type: s4
      - id: parm_start
        type: s4
      - id: locals
        type: s4
      - id: profile
        type: s4
      - id: s_name
        type: s4
      - id: s_file
        type: s4
      - id: num_parms
        type: s4
      - id: parm_size
        type: u1
        repeat: expr
        repeat-expr: 8
    instances:
      get_name:
        pos: s_name + _parent.header.ofs_strings
        type: strz
        encoding: ascii
      get_filename:
        pos: s_file + _parent.header.ofs_strings
        type: strz
        encoding: ascii

  progs_def_t:
    seq:
      - id: type
        type: u2
      - id: ofs
        type: u2
      - id: s_name
        type: u4
    instances:
      get_name:
        pos: s_name + _parent.header.ofs_strings
        type: strz
        encoding: ascii

  progs_statement_t:
    seq:
      - id: opcode
        type: u2
      - id: a
        type: s2
      - id: b
        type: s2
      - id: c
        type: s2

  progs_global_t:
    seq:
      - id: g
        size: 4

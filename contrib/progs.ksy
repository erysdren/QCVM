meta:
  id: quake_progs
  title: QuakeC Bytecode
  file-extension: dat
  license: CC0-1.0
  endian: le

doc: |
  This specification was authored by erysdren (it/its).
  https://erysdren.me/

seq:
  - id: header
    type: progs_header
  - id: extended_header
    type: progs_header_extended
    if: 'header.version == progs_versions::fte'

instances:

  strings:
    pos: header.ofs_strings
    size: header.num_strings

  functions:
    pos: header.ofs_functions
    repeat: expr
    repeat-expr: header.num_functions
    type:
      switch-on: _root.header.version
      cases:
        'progs_versions::qtest': progs_function_qtest
        'progs_versions::quake': progs_function
        'progs_versions::fte': progs_function

  fielddefs:
    pos: header.ofs_fielddefs
    repeat: expr
    repeat-expr: header.num_fielddefs
    type:
      switch-on: _root.header.version
      cases:
        'progs_versions::qtest': progs_def_qtest
        'progs_versions::quake': progs_def
        'progs_versions::fte': progs_def

  globaldefs:
    pos: header.ofs_globaldefs
    repeat: expr
    repeat-expr: header.num_globaldefs
    type:
      switch-on: _root.header.version
      cases:
        'progs_versions::qtest': progs_def_qtest
        'progs_versions::quake': progs_def
        'progs_versions::fte': progs_def

  statements:
    pos: header.ofs_statements
    repeat: expr
    repeat-expr: header.num_statements
    type:
      switch-on: _root.header.version
      cases:
        'progs_versions::qtest': progs_statement_qtest
        'progs_versions::quake': progs_statement
        'progs_versions::fte': progs_statement

  globals:
    pos: header.ofs_globals
    type: progs_global
    repeat: expr
    repeat-expr: header.num_globals

types:

  progs_header:
    seq:
      - id: version
        type: s4
        enum: progs_versions
      - id: crc
        type: s4
        enum: progs_crcs
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

  progs_header_extended:
    seq:
      - id: ofs_files
        type: s4
      - id: ofs_linenums
        type: s4
      - id: ofs_compressed_functions
        type: s4
      - id: num_compressed_functions
        type: s4
      - id: ofs_types
        type: s4
      - id: num_types
        type: s4
      - id: num_compressed_blocks
        type: s4
      - id: secondary_version
        type: u4
        enum: progs_versions_extended

  progs_function_qtest:
    seq:
      - id: first_statement
        type: s4
      - id: reserved
        type: s4
        repeat: expr
        repeat-expr: 3
      - id: s_name
        type: s4
      - id: s_file
        type: s4
      - id: num_parms
        type: s4
      - id: parm_start
        type: s4
      - id: parm_size
        type: s4
        repeat: expr
        repeat-expr: 8
    instances:
      name:
        pos: s_name + _parent.header.ofs_strings
        type: strz
        encoding: ascii
      filename:
        pos: s_file + _parent.header.ofs_strings
        type: strz
        encoding: ascii

  progs_function:
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
      name:
        pos: s_name + _parent.header.ofs_strings
        type: strz
        encoding: ascii
      filename:
        pos: s_file + _parent.header.ofs_strings
        type: strz
        encoding: ascii

  progs_def_qtest:
    seq:
      - id: type
        type: u4
      - id: s_name
        type: u4
      - id: ofs
        type: u4
    instances:
      name:
        pos: s_name + _parent.header.ofs_strings
        type: strz
        encoding: ascii

  progs_def:
    seq:
      - id: type
        type: u2
      - id: ofs
        type: u2
      - id: s_name
        type: u4
    instances:
      name:
        pos: s_name + _parent.header.ofs_strings
        type: strz
        encoding: ascii

  progs_statement_qtest:
    seq:
      - id: line
        type: u4
      - id: opcode
        type: u2
      - id: a
        type: s2
      - id: b
        type: s2
      - id: c
        type: s2

  progs_statement:
    seq:
      - id: opcode
        type: u2
      - id: a
        type: s2
      - id: b
        type: s2
      - id: c
        type: s2

  progs_global:
    seq:
      - id: g
        size: 4

enums:

  progs_versions:
    3: qtest
    6: quake
    7: fte

  progs_crcs:
    58535: qtest
    6956: quake091
    5927: quake101
    54730: quakeworld

  progs_versions_extended:
    0x021b1461: fte16
    0x65167402: fte32

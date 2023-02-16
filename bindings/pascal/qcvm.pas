unit qcvm;

interface

{ uses }
uses
	ctypes;

{ types }
type
	qcvm_t = pointer;
	qcvm_export_t = procedure(qcvm : qcvm_t); cdecl;
	qcvm_vec3 = record
		x : real;
		y : real;
		z : real;
	end;

{ qcvm_bootstrap.c }
function qcvm_open(filename : pchar) : qcvm_t; cdecl; external 'qcvm';
procedure qcvm_close(qcvm : qcvm_t); cdecl; external 'qcvm';

{ qcvm_runtime.c }
procedure qcvm_run(qcvm : qcvm_t; func : integer); cdecl; external 'qcvm';

{ qcvm_utils.c }
procedure qcvm_set_global_float(qcvm : qcvm_t; ofs : integer; val : real); cdecl; external 'qcvm';
procedure qcvm_set_global_vector(qcvm : qcvm_t; ofs : integer; a : real; b : real; c : real); cdecl; external 'qcvm';
function qcvm_get_global(qcvm : qcvm_t; gname : pchar) : integer; cdecl; external 'qcvm';
procedure qcvm_set_parm_string(qcvm : qcvm_t; parm : integer; s : pchar); cdecl; external 'qcvm';
procedure qcvm_set_parm_vector(qcvm : qcvm_t; parm : integer; a : real; b : real; c : real); cdecl; external 'qcvm';
procedure qcvm_set_parm_int(qcvm : qcvm_t; parm : integer; val : integer); cdecl; external 'qcvm';
procedure qcvm_set_parm_float(qcvm : qcvm_t; parm : integer; val : real); cdecl; external 'qcvm';
function qcvm_get_argc(qcvm : qcvm_t) : integer; cdecl; external 'qcvm';
function qcvm_get_parm_vector(qcvm : qcvm_t; parm : integer) : qcvm_vec3; cdecl; external 'qcvm';
function qcvm_get_parm_string(qcvm : qcvm_t; parm : integer) : pchar; cdecl; external 'qcvm';
function qcvm_get_parm_int(qcvm : qcvm_t; parm : integer) : integer; cdecl; external 'qcvm';
function qcvm_get_parm_float(qcvm : qcvm_t; parm : integer) : real; cdecl; external 'qcvm';
procedure qcvm_return_string(qcvm : qcvm_t; s : pchar); cdecl; external 'qcvm';
procedure qcvm_return_vector(qcvm : qcvm_t; a : real; b : real; c : real); cdecl; external 'qcvm';
procedure qcvm_return_int(qcvm : qcvm_t; val : integer); cdecl; external 'qcvm';
procedure qcvm_return_float(qcvm : qcvm_t; val : real); cdecl; external 'qcvm';
function qcvm_get_function(qcvm : qcvm_t; fname : pchar) : integer; cdecl; external 'qcvm';
procedure qcvm_add_export(qcvm : qcvm_t; exp : qcvm_export_t); cdecl; external 'qcvm';

{ qclib.c }
procedure qclib_install(qcvm : qcvm_t); cdecl; external 'qcvm';

implementation

end.

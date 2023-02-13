unit qcvm;

interface

uses
	ctypes;

type
	qcvm_t = pointer;
	qcvm_export_t = procedure(qcvm : qcvm_t); cdecl;

{ main qcvm functions }
function qcvm_open(filename : pchar) : qcvm_t; cdecl; external 'qcvm';
procedure qcvm_close(qcvm : qcvm_t); cdecl; external 'qcvm';
function qcvm_get_function(qcvm : qcvm_t; fname : pchar) : integer; cdecl; external 'qcvm';
procedure qcvm_run(qcvm : qcvm_t; func : integer); cdecl; external 'qcvm';
procedure qcvm_add_export(qcvm : qcvm_t; exp : qcvm_export_t); cdecl; external 'qcvm';

{ auxiliary qclib }
procedure qclib_install(qcvm : qcvm_t); cdecl; external 'qcvm';

implementation

end.

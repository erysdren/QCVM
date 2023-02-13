unit qcvm;

interface

uses
	ctypes;

type
	qcvm_t = ^longint;

{ main qcvm functions }
function qcvm_open(filename : pchar) : qcvm_t; cdecl; external 'libqcvm_shared.so';
procedure qcvm_close(qcvm : qcvm_t); cdecl; external 'libqcvm_shared.so';
function qcvm_get_function(qcvm : qcvm_t; fname : pchar) : integer; cdecl; external 'libqcvm_shared.so';
procedure qcvm_run(qcvm : qcvm_t; func : integer); cdecl; external 'libqcvm_shared.so';

{ auxiliary qclib }
procedure qclib_install(qcvm : qcvm_t); cdecl; external 'libqcvm_shared.so';

implementation

end.

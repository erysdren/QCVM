program qcvm_test;

uses
	qcvm;

const
	progs = 'testqc.dat';

var
	qc : qcvm_t;
	fn : integer;

begin

	{ talk to user }
	WriteLn('loading module ', progs);

	{ call c function }
	qc := qcvm_open(progs);

	{ test if module loaded }
	if qc = nil then
	begin
		WriteLn('failed to load ', progs);
	end
	else
	begin
		WriteLn('successfully loaded ', progs);
	end;

	{ install basic qclib functions }
	qclib_install(qc);

	{ get function id }
	fn := qcvm_get_function(qc, 'test');

	if fn < 1 then
	begin
		WriteLn('failed to find function "test()" in ', progs);
	end
	else
	begin
		WriteLn('running function test()');
		qcvm_run(qc, fn);
	end;

	{ clean up shop }
	WriteLn('closing up shop...');
	qcvm_close(qc);

end.

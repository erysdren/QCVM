
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test4.dat
#pragma autoproto

void say_hello_world() = #0 : say_hello_world;

void main()
{
	say_hello_world();
}

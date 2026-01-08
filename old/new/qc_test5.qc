
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test5.dat
#pragma autoproto

void say_hello_world() = #0 : say_hello_world;

void main()
{
	float f = 1 * 4;
	say_hello_world();

	f += 352;

	say_hello_world();
}

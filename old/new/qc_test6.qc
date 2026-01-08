
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test6.dat
#pragma autoproto

// builtins
void print(string s, ...) = #1 : print;
entity spawn() = #2 : spawn;

// define some fields
.float test0;
.vector test1;
.float test2;
.vector test3;
.string test4;
.entity test5;

// main
void main()
{
	entity e = spawn();
	e.test0 = 69;
	e.test2 = 42;
	e.test1 = [2, 2, 2];
	e.test3 = [3, 3, 3];

	float store_test0 = e.test0;

	if (store_test0 == 69)
	{
		print("69\n");
	}
	else
	{
		print("the world is ending\n");
	}
}

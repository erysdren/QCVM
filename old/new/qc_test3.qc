
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test3.dat
#pragma autoproto

void print(string s, ...) = #1 : print;

float func_with_arg(float in)
{
	return in * 2;
}

void main()
{
	float f = func_with_arg(2);
	print("hello world!\n");
}

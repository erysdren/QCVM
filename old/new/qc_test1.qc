
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test1.dat
#pragma autoproto

void print(string s, ...) = #1 : print;

void main()
{
	print("hello world!\n");
}

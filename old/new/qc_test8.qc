
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test8.dat
#pragma autoproto

// builtins
void print(string s, ...) = #1 : print;
entity spawn() = #2 : spawn;
string getenv(string name) = #3 : getenv;

// main
void() main =
{
	print("PWD: \"", getenv("PWD"), "\"\n");
};

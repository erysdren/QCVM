
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test2.dat
#pragma autoproto

void print(string s, ...) = #1 : print;
entity spawn() = #2 : spawn;

class MyClass {
	string classname;
	string targetname;
	void() MyClass;
	nonvirtual void() SayHello;
	virtual void() SayHi;
};

void MyClass::MyClass(void)
{
	print("hello world!\n");
}

void MyClass::SayHello(void)
{
	print("hello!\n");
}

void MyClass::SayHi(void)
{
	print("hi!\n");
}

void main()
{
	MyClass m = spawn(MyClass);
	m.SayHello();
	m.SayHi();
	m.classname = "MyClassName";

	print(m.classname, "\n");
}

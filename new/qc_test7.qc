
#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat qc_test7.dat
#pragma autoproto

// builtins
void print(string s, ...) = #1 : print;
entity spawn() = #2 : spawn;

// the mighty me
entity self;

// entity fields
.string classname;
.string targetname;
.void() think;

// setup monter frames
$frame walk1 walk2 walk3 walk4 walk5 walk6 walk7 walk8

void() monster_test_walk01 = [ $walk1, monster_test_walk02 ] { print("walk01\n"); };
void() monster_test_walk02 = [ $walk2, monster_test_walk03 ] { print("walk02\n"); };
void() monster_test_walk03 = [ $walk3, monster_test_walk04 ] { print("walk03\n"); };
void() monster_test_walk04 = [ $walk4, monster_test_walk05 ] { print("walk04\n"); };
void() monster_test_walk05 = [ $walk5, monster_test_walk06 ] { print("walk05\n"); };
void() monster_test_walk06 = [ $walk6, monster_test_walk07 ] { print("walk06\n"); };
void() monster_test_walk07 = [ $walk7, monster_test_walk08 ] { print("walk07\n"); };
void() monster_test_walk08 = [ $walk8, monster_test_walk01 ] { print("walk08\n"); };

// main
void() main =
{
	entity e = spawn();

	e.classname = "monster_test";
	e.targetname = "bob";
	e.think = monster_test_walk01;

	print("spawned entity\n");

	e.think();
};

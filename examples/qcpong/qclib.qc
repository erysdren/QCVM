
//
//
// qclib exports
//
//

// macros
#define TRUE 1
#define FALSE 0

// print string to stdoout
void print(string s, ...) = #1;

// acquire entity handle from the engine
entity spawn() = #2;

// get length of string
float strlen(string s) = #3;

// join two strings 
string strcat(string s1, string s2, ...) = #4;

// float to string 
string ftos(float f) = #5;

// vector to string
string vtos(vector v) = #6;

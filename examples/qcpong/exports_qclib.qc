
//
//
// qclib exports
//
//

//
// text macros
//

// true/false
#define TRUE 1
#define FALSE 0

//
// functions
//

void print(string s, ...) = #0 : print;
entity spawn() = #0 : spawn;
float strlen(string s) = #0 : strlen;
string strcat(string s1, string s2, ...) = #0 : strcat;
string ftos(float f) = #0 : ftos;
string vtos(vector v) = #0 : vtos;
string substring(string s, float start, float len) = #0 : substring;

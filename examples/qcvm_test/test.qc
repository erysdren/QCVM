//==============================================================================
//
// MIT License
//
// Copyright (c) 2023 erysdren (it/she)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
//==============================================================================

// qclib
void print(string s, ...) = #0 : print;
entity spawn() = #0 : spawn;
float strlen(string s) = #0 : strlen;
string strcat(string s1, string s2, ...) = #0 : strcat;
string ftos(float f) = #0 : ftos;
string vtos(vector v) = #0 : vtos;
string substring(string s, float start, float len) = #0 : substring;

// ent fields
.float field3;
.float field4;
.float field6;
.float field1;
.string field2;

// globals
entity gtest;

void test()
{
	entity ent;
	float x, y, z;

	gtest.field1 = 4643;
	gtest.field2 = "hello world!";

	x = 1;
	y = x * 4;
	z = y * y;
	ent = spawn();
	ent.field1 = 352;
	ent.field2 = "hello!!!";

	print("sup\n");
	print(ent.field2, "\n");
}

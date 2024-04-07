//==============================================================================
//
// MIT License
//
// Copyright (c) 2023-2024 erysdren (it/she)
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

void module_main()
{
	print("hello world from module 1!\n");

	sendstring("i am module 1, sending a message to module 2\n");

	sendfloat(462.7);
}

void module_recieve(__variant data, float type)
{
	switch (type)
	{
		case RECIEVE_STRING:
			print("module 1 recieved string: ", (string)data, "\n");
			break;

		case RECIEVE_FLOAT:
			print("module 1 recieved float: ", ftos((float)data), "\n");
			break;
	}
}

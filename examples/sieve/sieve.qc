//==============================================================================
//
// MIT License
//
// Copyright (c) 2023-2026 erysdren (it/its)
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

#pragma target vanilla
#pragma warning disable Q208
#pragma progs_dat sieve.dat
#pragma flag enable fastarrays
#pragma autoproto

#define N 300

void print(string s, ...) = #1 : print;

float(void) sieve =
{
	float num_primes;
	float primes[N + 1];
	float i, p;

	print("initializing array...\n");

	// initialize array
	for (i = 0; i < N; i++)
		primes[i] = 1;

	print("calculating primes...\n");

	// calculate primes
	for (p = 2; p * p <= N; p++)
	{
		if (primes[p] == 1)
		{
			for (i = p * p; i <= N; i += p)
				primes[i] = 0;
		}
	}

	print("counting primes...\n");

	// count up number of primes
	num_primes = 0;
	for (p = 2; p <= N; p++)
		if (primes[p])
			num_primes += 1;

	return num_primes;
};

float(void) main =
{
	return sieve();
}


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

#include "Benchmarks.hpp"

int main()
{
	{
		BaselineBenchmark b;
		b.Run(1000.0f, 5);
	}
	{
		MovementBenchmark b;
		b.Run(1000.0f, 5);
	}
	{
		PlainComponentBenchmark b;
		b.Run(1000.0f, 5);
	}
	{
		InsertRemoveBenchmark b;
		b.Run(1000.0f, 5);
	}

	return 0;
}

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>

using namespace std::chrono;

void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n",
		maximum->get_reference(), maximum->get_index_reference());
}
void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Min element = %d has index = %d\n",
		minimum->get_reference(), minimum->get_index_reference());
}
void ParallelSort(int *begin, int *end)
{
	
	if (begin != end)
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
	
	
}
unsigned int stopwatch()
{
	static auto start_time = std::chrono::steady_clock::now();
	auto end_time = std::chrono::steady_clock::now();
	auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	start_time = end_time;
	return delta.count();
}

void  CompareForAndCilk_For(size_t sz) {

	std::vector<int> vect;
	stopwatch();
	for (int i = 0; i < sz; i++) {
		vect.push_back(rand() % 20000 + 1);
	}
	printf("\n Loop FOR duration is: %d seconds \n", stopwatch());

	cilk::reducer<cilk::op_vector<int>>red_vec;
	stopwatch();
	for (int j = 0; j < sz; j++) {
		red_vec->push_back(rand() % 20000 + 1);
	}
	printf("\n Loop CILK_FOR duration is: %d seconds \n", stopwatch());
}


int main()
{
	srand((unsigned)time(0));

	__cilkrts_set_param("nworkers", "4");

	long i;
	int mass_size[4] = { 10000, 100000, 500000, 1000000 };

	printf("Task2\n");
	for (i = 0; i < 2; i++) {

		int *mass_begin, *mass_end;
		int *mass = new int[mass_size[0]];
		for (int j = 0; j < mass_size[0]; j++)
		{
			mass[j] = (rand() % 25000) + 1;
		}
		mass_begin = mass;
		mass_end = mass_begin + mass_size[0];
		if (i == 0) {
			printf("\nCheck Max\n");
			ReducerMaxTest(mass, mass_size[0]);
			ParallelSort(mass_begin, mass_end);
			ReducerMaxTest(mass, mass_size[0]);
		}
		else {
			printf("\nCheck Min\n");
			ReducerMinTest(mass, mass_size[0]);
			ParallelSort(mass_begin, mass_end);
			ReducerMinTest(mass, mass_size[0]);
			printf("--------------------- \n");
			
		};
		delete[]mass;
	};


	printf("Task 3\n");
	
	for (i = 0; i < 4; i++) {

		int *mass_begin, *mass_end;
		int *mass = new int[mass_size[i]];
		for (int j = 0; j < mass_size[i]; ++j)
		{
			mass[j] = (rand() % 25000) + 1;
		}
		mass_begin = mass;
		mass_end = mass_begin + mass_size[i];
		printf("masSize = %d \n", mass_size[i]);
		printf("\nBefore sorting\n");
		ReducerMinTest(mass, mass_size[i]);
		ReducerMaxTest(mass, mass_size[i]);
		stopwatch();
		ParallelSort(mass_begin, mass_end);
		printf("\nDuration is: %d seconds \n", stopwatch());
		printf("\nAfter sorting\n");
		ReducerMinTest(mass, mass_size[i]);
		ReducerMaxTest(mass, mass_size[i]);
		printf("--------------------- \n");
		delete[]mass;
	};

	printf("Task 4\n");

	int sz[8] = { 1000000, 100000, 10000, 1000, 500, 100, 50, 10};
	for (i = 7; i >=0 ; i--) {
		printf("\n sz = %d \n", sz[i]);
		CompareForAndCilk_For(sz[i]);
		printf("----------------------");
	}

	return 0;
}

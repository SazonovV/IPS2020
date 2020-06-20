#include <vector>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <exception>
#include <locale.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cilk/cilk.h>
#include "cilk/reducer.h"
#include <cilk/reducer_opadd.h>
#include <tbb/tbb.h>
#include <tbb/parallel_reduce.h>
#include <chrono>
#include <ctime>
using namespace std;
using namespace std::chrono;

double integrand(double x) {
	return 8 / (1 + pow(x, 2));
}

double teoretical = M_PI * 2;
double right_square_integral(int accuracy, double a, double b)
{
	//init values
	double integral = 0;
	double step = (b - a) / double(accuracy);
	//start timer
	auto start = high_resolution_clock::now();
	//main cycle
	for (int i = 0; i < accuracy; i++)
		integral += integrand(a + double(i) / double(accuracy)) * step;
	//end time
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	//print output
	cout<< "Value: " << integral << ". Time: " << duration.count() << " microseconds. Diff: " << abs(teoretical - integral) << endl;
	return integral;
}

void  threading_right_square_integral(double *integral, double int_step, double a, double b)
{
	for (double i = a + int_step; i <= b; i += int_step)
		*integral += integrand(i) * int_step;
}

double threadint_integral(int accuracy, double a, double b)
{
	int n = 4;
	double integral = 0;
	double step = (b - a) / n;
	double int_step = (b - a) / double(accuracy);
	vector<thread> threads(n);
	vector<double> integrals(n);

	// initialize threads
	auto start = high_resolution_clock::now();
	for (int i = 0; i < n; i++)
		threads[i] = thread(threading_right_square_integral, &integrals[i], int_step, a + i * step, a + (i + 1) * step);

	//wait for threads finish and sum up results
	for (int i = 0; i < n; i++)
	{
		threads[i].join();
		integral += integrals[i];
	}

	// print time, return results
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Value: " << integral;
	cout << ". Time: "
		<< duration.count() << " microseconds. Diff: " << abs(teoretical - integral) << endl;
	return integral;

}

double cilk_right_square_integral(int accuracy, double a, double b)
{
	//init values
	double integral;
	cilk::reducer_opadd<double> sum(0.0);
	double step = (b - a) / double(accuracy);
	//start timer
	auto start = high_resolution_clock::now();
	//main cycle
	cilk_for(int i = 0; i < accuracy; i++) {
		sum += integrand(a + double(i) / double(accuracy)) * step;
	}
	//end time
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	//print output
	integral = sum.get_value();
	cout << "Value: " << integral << ". Time: " << duration.count() << " microseconds. Diff: " << abs(teoretical - integral) << endl;
	return integral;
}

double tbb_right_square_integral(int accuracy, double a, double b)
{
	//init values
	double integral = 0;
	double step = (b - a) / double(accuracy);
	//start timer
	auto start = high_resolution_clock::now();
	//main cycle
	double sum = tbb::parallel_reduce(tbb::blocked_range<int>(0, accuracy - 1),
		0.0,
		[&](tbb::blocked_range<int> r, double sum)
	{
		for(int i = r.begin(); i != r.end(); ++i)
			sum += integrand(a + double(i) / double(accuracy));
		return sum;
		},				
		plus<double>()
	);
	//end time
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	//print output
	integral = sum * step;
	cout << "Value: " << integral << ". Time: " << duration.count() << " microseconds. Diff: " << abs(teoretical - integral) << endl;
	return integral;
}

int main ()
{
	double a = 0;
	double b = 1;
	int accuracy[5] = { 100, 1000, 10000, 100000, 1000000 };

	for (int acc : accuracy)
	{
		cout << "ACCURACY : " << acc << endl;
		cout << "Sequential : " << endl;
		right_square_integral(acc, a, b);
		cout << "Threading : " << endl;
		threadint_integral(acc, a, b);
		cout << "Threading with cilk : " << endl;
		cilk_right_square_integral(acc, a, b);
		cout << "Threading with TBB : " << endl;
		tbb_right_square_integral(acc, a, b);
		cout << endl;
	}
	return 0;
}

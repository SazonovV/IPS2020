#include <stdio.h>
#include <ctime>
#include "cilk\cilk.h"
#include "cilk\reducer_opadd.h"

#include <chrono>

using namespace std::chrono;

unsigned int stopwatch()
{
	static auto start_time = std::chrono::steady_clock::now();
	auto end_time = std::chrono::steady_clock::now();
	auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	start_time = end_time;
	return delta.count();
}


const int MATRIX_SIZE = 1500;


void InitMatrix(double** matrix)
{
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}

	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		for (int j = 0; j <= MATRIX_SIZE; ++j)
		{
			matrix[i][j] = rand() % 2500 + 1;
		}
	}
}


void CilkGaussMethod(double **matrix, const int rows, double* result) {
	int k;
	double koef;

	stopwatch();
	for (k = 0; k < rows; ++k)
	{
		//
		int i = 0;
		cilk_for(i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			};
		};
	};
	
	printf("\nParallel forward elimination duration: %d seconds \n", stopwatch());

	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		//
		cilk_for(int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		};

		result[k] /= matrix[k][k];
	};
}
void SerialGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;
	stopwatch();
	
	for (k = 0; k < rows; ++k)
	{

		for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}

	}
	
	printf("Serial forward elimination duration: %d seconds \n", stopwatch());
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}
}


int main()
{
	srand((unsigned)time(0));

	int i;

	const int test_matrix_lines = 4;
	double **test_matrix = new double*[test_matrix_lines];
	for (i = 0; i < test_matrix_lines; ++i)
	{
		test_matrix[i] = new double[test_matrix_lines + 1];
	}


	double **matrix = new double*[MATRIX_SIZE];
	InitMatrix(matrix);
	double **matrix1 = new double*[MATRIX_SIZE];
	InitMatrix(matrix1);
	double *result = new double[MATRIX_SIZE];

	test_matrix[0][0] = 2; test_matrix[0][1] = 5;  test_matrix[0][2] = 4;  test_matrix[0][3] = 1;  test_matrix[0][4] = 20;
	test_matrix[1][0] = 1; test_matrix[1][1] = 3;  test_matrix[1][2] = 2;  test_matrix[1][3] = 1;  test_matrix[1][4] = 11;
	test_matrix[2][0] = 2; test_matrix[2][1] = 10; test_matrix[2][2] = 9;  test_matrix[2][3] = 7;  test_matrix[2][4] = 40;
	test_matrix[3][0] = 3; test_matrix[3][1] = 8;  test_matrix[3][2] = 9;  test_matrix[3][3] = 2;  test_matrix[3][4] = 37;

	
	printf("Serial method on matrix with size = MATRIX_SIZE:\n ");
	SerialGaussMethod(matrix, MATRIX_SIZE, result);
	printf("\nParallel method on test matrix:\n ");
	CilkGaussMethod(test_matrix, test_matrix_lines, result);
	printf("Parallel method on matrix with size = MATRIX_SIZE:\n ");
	CilkGaussMethod(matrix, MATRIX_SIZE, result);
	
	//printf("\nForward elimination duration\n For serial function: %d \nFor cilk function: %d\n");
	//printf("Cilk function is faster in %d times", cilkGaussTime / serialGauesTime);
	//CilkGaussMethod(test_matrix, test_matrix_lines, result, cilkGaussTime);
	for (i = 0; i < MATRIX_SIZE; ++i)
	{
		delete[]matrix[i];
	}

	for (i = 0; i < test_matrix_lines; ++i)
	{
		delete[]test_matrix[i];
	}

	printf("Solution:\n");

	for (i = 0; i < MATRIX_SIZE; ++i)
	{
		printf("x(%d) = %lf\n", i, result[i]);
	}

	delete[] result;

	return 0;
}

#include <vector>
#include <thread>
#include <stdio.h>
#include <exception>
#include <locale.h>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>
#include <ctime>
#include "time.h"
#define _CRT_SECURE_NO_WARNINGS

#define _USE_MATH_DEFINES

#include <immintrin.h>

#include <malloc.h>

#include <locale.h>

int newRand()
{

	unsigned __int16 r;
	_rdrand16_step(&r);
	return r;
}

/// ÔÂÂ˜ËÒÎÂÌËÂ, ÓÔÂ‰ÂÎˇ˛˘ÂÂ Í‡Í ·Û‰ÂÚ ÔÓËÒıÓ‰ËÚ¸ ‚˚˜ËÒÎÂÌËÂ
/// ÒÂ‰ÌËı ÁÌ‡˜ÂÌËÈ Ï‡ÚËˆ˚: ÔÓ ÒÚÓÍ‡Ï ËÎË ÔÓ ÒÚÓÎ·ˆ‡Ï
enum class eprocess_type
{
	by_rows = 0,
	by_cols
};

void InitMatrix(double** matrix, const size_t numb_rows, const size_t numb_cols)
{
	for (size_t i = 0; i < numb_rows; ++i)
	{
		for (size_t j = 0; j < numb_cols; ++j)
		{
			matrix[i][j] = newRand() % 5 + 1;
		}
	}
}

/// ‘ÛÌÍˆËˇ PrintMatrix() ÔÂ˜‡Ú‡ÂÚ ˝ÎÂÏÂÌÚ˚ Ï‡ÚËˆ˚ <i>matrix</i> Ì‡ ÍÓÌÒÓÎ¸;
/// numb_rows - ÍÓÎË˜ÂÒÚ‚Ó ÒÚÓÍ ‚ ËÒıÓ‰ÌÓÈ Ï‡ÚËˆÂ <i>matrix</i>
/// numb_cols - ÍÓÎË˜ÂÒÚ‚Ó ÒÚÓÎ·ˆÓ‚ ‚ ËÒıÓ‰ÌÓÈ Ï‡ÚËˆÂ <i>matrix</i>
void PrintMatrix(double** matrix, const size_t numb_rows, const size_t numb_cols)
{
	printf("Generated matrix:\n");
	for (size_t i = 0; i < numb_rows; ++i)
	{
		for (size_t j = 0; j < numb_cols; ++j)
		{
			printf("%lf ", matrix[i][j]);
		}
		printf("\n");
	}
}

/// ‘ÛÌÍˆËˇ FindAverageValues() Ì‡ıÓ‰ËÚ ÒÂ‰ÌËÂ ÁÌ‡˜ÂÌËˇ ‚ Ï‡ÚËˆÂ <i>matrix</i>
/// ÔÓ ÒÚÓÍ‡Ï, ÎË·Ó ÔÓ ÒÚÓÎ·ˆ‡Ï ‚ Á‡‚ËÒËÏÓÒÚË ÓÚ ÁÌ‡˜ÂÌËˇ Ô‡‡ÏÂÚ‡ <i>proc_type</i>;
/// proc_type - ÔËÁÌ‡Í, ‚ Á‡‚ËÒËÏÓÒÚË ÓÚ ÍÓÚÓÓ„Ó ÒÂ‰ÌËÂ ÁÌ‡˜ÂÌËˇ ‚˚˜ËÒÎˇ˛ÚÒˇ 
/// ÎË·Ó ÔÓ ÒÚÓÍ‡Ï, ÎË·Ó ÔÓ ÒÚÓ·ˆ‡Ï ËÒıÓ‰ÌÓÈ Ï‡ÚËˆ˚ <i>matrix</i>
/// matrix - ËÒıÓ‰Ì‡ˇ Ï‡ÚËˆ‡
/// numb_rows - ÍÓÎË˜ÂÒÚ‚Ó ÒÚÓÍ ‚ ËÒıÓ‰ÌÓÈ Ï‡ÚËˆÂ <i>matrix</i>
/// numb_cols - ÍÓÎË˜ÂÒÚ‚Ó ÒÚÓÎ·ˆÓ‚ ‚ ËÒıÓ‰ÌÓÈ Ï‡ÚËˆÂ <i>matrix</i>
/// average_vals - Ï‡ÒÒË‚, ÍÛ‰‡ ÒÓı‡Ìˇ˛ÚÒˇ ‚˚˜ËÒÎÂÌÌ˚Â ÒÂ‰ÌËÂ ÁÌ‡˜ÂÌËˇ
void FindAverageValues(eprocess_type proc_type, double** matrix, const size_t numb_rows, const size_t numb_cols, double* average_vals)
{
	switch (proc_type)
	{
	case eprocess_type::by_rows:
	{
		cilk_for (size_t i = 0; i < numb_rows; ++i)
		{
			cilk::reducer_opadd<double> sum(0.0);
			cilk_for(size_t j = 0; j < numb_cols; ++j)
			{
				sum += matrix[i][j];
			}
			average_vals[i] = sum / numb_cols;
		}
		break;
	}
	case eprocess_type::by_cols:
	{
		cilk_for (size_t j = 0; j < numb_cols; ++j)
		{
			cilk::reducer_opadd<double> sum(0.0);
			cilk_for(size_t i = 0; i < numb_rows; ++i)
			{
				sum += matrix[i][j];
			}
			average_vals[j] = sum / numb_rows;
		}
		break;
	}
	default:
	{
		throw("Incorrect value for parameter 'proc_type' in function FindAverageValues() call!");
	}
	}
}

/// ‘ÛÌÍˆËˇ PrintAverageVals() ÔÂ˜‡Ú‡ÂÚ ˝ÎÂÏÂÌÚ˚ Ï‡ÒÒË‚‡ <i>average_vals</i> Ì‡ ÍÓÌÒÓÎ¸;
/// proc_type - ÔËÁÌ‡Í, ÓÚ‚Â˜‡˛˘ËÈ Á‡ ÚÓ, Í‡Í ·˚ÎË ‚˚˜ËÒÎÂÌ˚ 
/// ÒÂ‰ÌËÂ ÁÌ‡˜ÂÌËˇ ËÒıÓ‰ÌÓÈ Ï‡ÚËˆ˚ ÔÓ ÒÚÓÍ‡Ï ËÎË ÔÓ ÒÚÓÎ·ˆ‡Ï
/// average_vals - Ï‡ÒÒË‚, ı‡Ìˇ˘ËÈ ÒÂ‰ÌËÂ ÁÌ‡˜ÂÌËˇ ËÒıÓ‰ÌÓÈ Ï‡ÚËˆ˚,
/// ‚˚˜ËÒÎÂÌÌ˚Â ÔÓ ÒÚÓÍ‡Ï ËÎË ÔÓ ÒÚÓÎ·ˆ‡Ï
/// dimension - ÍÓÎË˜ÂÒÚ‚Ó ˝ÎÂÏÂÌÚÓ‚ ‚ ËÒıÓ‰ÌÓÈ Ï‡ÒÒË‚Â <i>average_vals</i>
void PrintAverageVals(eprocess_type proc_type, double* average_vals, const size_t dimension)
{
	switch (proc_type)
	{
	case eprocess_type::by_rows:
	{
		printf("\nAverage values in rows:\n");
		for (size_t i = 0; i < dimension; ++i)
		{
			printf("Row %u: %lf\n", i, average_vals[i]);
		}
		break;
	}
	case eprocess_type::by_cols:
	{
		printf("\nAverage values in columns:\n");
		for (size_t i = 0; i < dimension; ++i)
		{
			printf("Column %u: %lf\n", i, average_vals[i]);
		}
		break;
	}
	default:
	{
		throw("Incorrect value for parameter 'proc_type' in function PrintAverageVals() call!");
	}
	}
}


int main()
{
	const unsigned ERROR_STATUS = -1;
	const unsigned OK_STATUS = 0;

	unsigned status = OK_STATUS;

	try
	{
		
		srand((unsigned)time(0));

		const size_t numb_rows = 2;
		const size_t numb_cols = 3;

		double** matrix = new double*[numb_rows];
		for (size_t i = 0; i < numb_rows; ++i)
		{
			matrix[i] = new double[numb_cols];
		}

		double* average_vals_in_rows = new double[numb_rows];
		double* average_vals_in_cols = new double[numb_cols];

		InitMatrix(matrix, numb_rows, numb_cols);

		PrintMatrix(matrix, numb_rows, numb_cols);

		std::thread first_thr(FindAverageValues, eprocess_type::by_rows, matrix, numb_rows, numb_cols, average_vals_in_rows);
		std::thread second_thr(FindAverageValues, eprocess_type::by_cols, matrix, numb_rows, numb_cols, average_vals_in_cols);

		first_thr.join();
		second_thr.join();

		PrintAverageVals(eprocess_type::by_rows, average_vals_in_rows, numb_rows);
		PrintAverageVals(eprocess_type::by_cols, average_vals_in_cols, numb_cols);
	}

	catch (std::exception& except)
	{
		printf("Error occured!\n");
		except.what();
		status = ERROR_STATUS;
	}
	
	return status;
}

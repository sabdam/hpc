#include <iostream>
#include <random>
#include <ctime>
#include "mpi.h"

#define SIZE_MATRIX 500

using namespace std;

double* get_SLAU(int dim);
void slau(int argc, char* argv[]);

int main(int argc, char* argv[]) 
{
	slau(argc, argv);
	return 0;
}

// инициализация СЛАУ
double* get_SLAU(int dim) {
	srand(NULL);
	double* res = new double[dim*dim];
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++)
			res[i * dim + j] = (double)rand() / RAND_MAX * (100 + 100);
	}
	return res;
}

void slau(int argc, char* argv[]) {
	double time;
	int proc_rank, proc_num;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

	// количество строчек на процесс
	int current_row_num = SIZE_MATRIX / proc_num; 
    // массив коэффициентов
	double* matrix = get_SLAU(SIZE_MATRIX);
	// массив частей в процессах
	double* current_matrix = new double[SIZE_MATRIX * current_row_num]; 

	MPI_Scatter(matrix, SIZE_MATRIX * current_row_num, MPI_DOUBLE, current_matrix, SIZE_MATRIX * current_row_num, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// массив строк процесса
	double* rows = new double[SIZE_MATRIX * current_row_num];

	if (proc_rank == 0) {
		time = MPI_Wtime();
	}

	for (int i = 0; i < (proc_rank * current_row_num); i++) {
		MPI_Bcast(rows, SIZE_MATRIX, MPI_DOUBLE, i / current_row_num, MPI_COMM_WORLD); 

		for (int j = 0; j < current_row_num; j++) {
			for (int k = i + 1; k < SIZE_MATRIX; k++) {
				// редукция
				current_matrix[j * SIZE_MATRIX + k] -= current_matrix[j * SIZE_MATRIX + i] * rows[k];
			}
			// обнуляем элементы под главной диагональю
			current_matrix[j * SIZE_MATRIX + i] = 0;
		}
	}

	int current_col;
	// нормируем строки
	for (int i = 0; i < current_row_num; i++) {
		// текущий столбец процесса
		current_col = proc_rank * current_row_num + i;
		for (int j = current_col; j < SIZE_MATRIX; j++) {
			current_matrix[i * SIZE_MATRIX + j] /= current_matrix[i * SIZE_MATRIX + current_col];
		}

		// главная диагональ = 1
		current_matrix[i * SIZE_MATRIX + current_col] = 1;

		// копируем результат метода Гаусса 
		memcpy(rows, current_matrix + (i * SIZE_MATRIX), SIZE_MATRIX * sizeof(double));
		// передаем полученные строки
		MPI_Bcast(rows, SIZE_MATRIX, MPI_DOUBLE, proc_rank, MPI_COMM_WORLD);

		// редукция полученных строк
		for (int j = i + 1; j < current_row_num; j++) {
			for (int k = current_col + 1; k < SIZE_MATRIX; k++) {
				current_matrix[j * SIZE_MATRIX + k] -= current_matrix[j * SIZE_MATRIX + i] * rows[k];
			}
			// обнуление элементов под главной диагональю
			current_matrix[j * SIZE_MATRIX + current_col] = 0;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (proc_rank == 0) {
		time = MPI_Wtime() - time;
	}

	// собираем результат
	MPI_Gather(current_matrix, SIZE_MATRIX * current_row_num, MPI_DOUBLE, matrix, SIZE_MATRIX * current_row_num, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (proc_rank == 0) {
		cout << "Size = " << SIZE_MATRIX << "\nTime: " << time << "second" << endl;
	}

	MPI_Finalize();

	delete[] matrix;
	delete[] current_matrix;
	delete[] rows;
}

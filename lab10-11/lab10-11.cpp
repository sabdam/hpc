#include <mpi.h>
#include <math.h>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <vector>
#include <ctime>

using namespace std;

void true_reduce_scatter(int argc, char **argv);
void reduce_scatter(int argc, char **argv);
void my_reduce_scatter(void *sendbuf, int send_size, MPI_Datatype send_datatype, void *recvbuf, int recv_size, MPI_Datatype recv_datatype, MPI_Comm communicator);

int main(int argc, char **argv)
{
	true_reduce_scatter(argc, argv);
	//reduce_scatter(argc, argv);
	return 0;
}

void my_reduce_scatter(void *sendbuf, int send_size, MPI_Datatype send_datatype, void *recvbuf, int recv_size, MPI_Datatype recv_datatype, MPI_Comm communicator)
{
	int me, nprocs, left, right, index, index_next, i, j, typesize;
	char *tmp_recv, **tmp_send;

	MPI_Status status;
	MPI_Request request;

	MPI_Comm_rank(communicator, &me);
	MPI_Comm_size(communicator, &nprocs);

	MPI_Type_size(send_datatype, &typesize);

	tmp_recv = (char*)malloc(recv_size * typesize);
	tmp_send = (char **)malloc((nprocs + 1) * sizeof(char *));

	// заполнение адресов
	tmp_send[0] = (char *)sendbuf;
	for (i = 0; i < nprocs; i++)
		tmp_send[i + 1] = tmp_send[i] + (send_size * typesize);

	left = (me - 1 + nprocs) % nprocs;
	right = (me + 1) % nprocs;

	index = (me + 1) % nprocs;
	for (i = 1; i < nprocs; i++)
	{
		index_next = (index + 1) % nprocs;

		MPI_Irecv(tmp_recv, recv_size * typesize, MPI_CHAR, right, MPI_ANY_TAG, communicator, &request);
		MPI_Send(tmp_send[index], send_size * typesize, MPI_CHAR, left, 0, communicator);

		// дожидаемся получения
		MPI_Wait(&request, &status);

		// выполнение операции сложения
		// добавляем содержимое tmp_recv в tmp_send[index_next]
		for (j = 0; j < recv_size; j++)
		{
			if (i == (nprocs - 1))
				((int *)recvbuf)[(index_next*send_size) + j] =
				((int *)tmp_send[index_next])[j] + ((int *)tmp_recv)[j];
			else
				((int *)tmp_send[index_next])[j] += ((int *)tmp_recv)[j];
		}
		// передвигаем индекс
		index = index_next;
	}

	free(tmp_recv);
	free(tmp_send);
}

void reduce_scatter(int argc, char **argv) {
	int proc_rank, proc_num, typesize, sendsize;
    double t1 = 0, t2 = 0, dt = 0;

	MPI_Status status;
	// инициализация
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Type_size(MPI_INT, &typesize);

	if (proc_rank == 0) t1 = MPI_Wtime();

	int* sendbuf = new int[proc_num];
	int* recvbuf = new int[proc_num];

	for (int i = 0; i < proc_num; i++)
		sendbuf[i] = 1 + proc_rank + proc_num * i;

	sendsize = 10 / proc_num;

	my_reduce_scatter(sendbuf, sendsize, MPI_INT, recvbuf, sendsize, MPI_INT, MPI_COMM_WORLD);

	if (proc_rank == 0) {
		t2 = MPI_Wtime();
		dt = t2 - t1;
		cout << "Time: " << dt << "s\n";
	}

	MPI_Finalize();
}

void true_reduce_scatter(int argc, char **argv) {
	int proc_rank, proc_num, i, n;
	double t1 = 0, t2 = 0, dt = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

	if (proc_rank == 0) t1 = MPI_Wtime();

	int* sendbuf = new int[proc_num];
	int recvbuf;

	for (int i = 0; i < proc_num; i++)
		sendbuf[i] = 1 + proc_rank + proc_num * i;

	/*cout<< "Proc " << proc_rank << ": ";
	for (int i = 0; i < proc_num; i++)
		cout << sendbuf[i] << " ";
	cout << endl;*/

	int* recvcounts = new int[proc_num];
	for (int i = 0; i < proc_num; i++)
		recvcounts[i] = 1;

	MPI_Reduce_scatter(sendbuf, &recvbuf, recvcounts, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	//cout << "Proc "<< proc_rank << ": " << recvbuf << endl;

	if (proc_rank == 0) {
		t2 = MPI_Wtime();
		dt = t2 - t1;
		cout << "Time: " << dt << "s\n";
	}

	MPI_Finalize();
}


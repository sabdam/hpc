#include <iostream>
#include <string>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {

	int p_rank, p_size;
	string str1 = "This text to decrypt+-.";
    string str2 = "This text to decrypt+-.";

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p_size);

	if (p_rank == 0){
        double t, t1, t2;
        t1 = MPI_Wtime();

        for (int i = 1; i < p_size; i++){
            MPI_Send(str1.c_str(), str1.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < p_size; i++){
            MPI_Recv(&str1, str1.size(), MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        t2 = MPI_Wtime();
        t = t2 - t1;
        cout << "Res = " << str2 << endl;
        cout << "Time: " << t << "s\n";
     }

     else{

        MPI_Recv(&str1, str1.size(), MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        int k = str1.size() / (p_size - 1);
        int first = k * (p_rank - 1);
        int last = k * p_rank;

        if (p_rank == p_size -1) last = str1.size();

        for (int i = first; i < last; ++i){
            str2[i] = --str1[i];
            cout << str2[i];
        }
        cout << "\n";

        MPI_Send(str2.c_str(), str2.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

   MPI_Finalize();
  return 0;
}

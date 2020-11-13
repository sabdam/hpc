#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

int main()
{
    int N = 0, nThreads = 0;
  
    std::cout << "Enter N of matrix (NxN): ";
    std::cin >> N;
  
    std::vector<std::vector<double>> matrix(N, std::vector<double>(N));
  
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            matrix[i][j] = (rand() % 100);
        } 
    }
    
    for(auto& row: matrix)
    {
        for(auto& col : row)
        { 
            printf("%8.2f", col);
        }
        std::cout << "\n";
    }
    std::cout <<"\n";
 
    //matrix = {{2, 4, 6, 8}, {1, 2, 3, 4}, {1, 1, 1, 1}, {0, 0, 0, 0}};  
    //rank = 2

    std::cout << "Num threads: ";
    std::cin >> nThreads;


    if (nThreads > 0 && nThreads <= omp_get_max_threads())
    {
        omp_set_num_threads(nThreads);

        const double EPS = 1E-9;

        int rank = N;
        std::vector<char> line_used (N);
 
        int i, j, p, k;
        for (i=0; i<N; ++i) {
	        for (j=0; j<N; ++j)
		        if (!line_used[j] && abs(matrix[j][i]) > EPS)
			        break;
	        if (j == N)
		    --rank;
	        else { 
		        line_used[j] = true;
		        for (p=i+1; p<N; ++p)
			        matrix[j][p] /= matrix[j][i];
		        for (k=0; k<N; ++k)
			        if (k != j && abs (matrix[k][i]) > EPS)
			        #pragma omp parallel for shared(matrix) 
				        for (p=i+1; p<N; ++p)
					        matrix[k][p] -= matrix[j][p] * matrix[k][i];
    		}
    	}
    	
        std::cout << "Rank "<< rank;
        std::cout << "\n";
    }
    else
    { 
        printf("Incorrent input");
        getchar();
    }

    return(0);

}

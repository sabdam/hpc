#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#define N 1000

bool isprime(int n)
{
  int c = 0;
  int d;
  if (n==1) 
    return false; 
  for (d=2; d*d<=n; d++)
  {
    if (n%d==0)
      c++;
  } 
  if (c>0) return false;
  return true;
}

main()
{
  int A[N];
  int count = 0;  
  int nThreads;
  
  for (int i=1; i<N; i++)
    { 
      A[i] = i+1;  
    }

  std::cout << "Num threads: ";
  std::cin >> nThreads;
  if (nThreads > 0 && nThreads <= omp_get_max_threads())
   {
    omp_set_num_threads(nThreads);
  
    int i;
    #pragma omp parallel for schedule(static) private(i) shared(A) \
      reduction(+:count)
    for (i=1; i<N; i++)
    {
      if (isprime(A[i]) == true)
        count++;
    }

    std::cout << "Number of primes: "<< count;
  }
  else
  { 
    printf("Incorrent input");
    getch();
  }

  return(0);
}



 




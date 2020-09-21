#include <iostream>

#define N 1000

bool isprime(int n)
{
  int d;
  if (n==1) 
    return false; 
  for (d=2; d*d<=n; d++)
  {
    if (n%d==0)
      return false;
  } 
  return true;
}

main()
{
  int A[N];
  int count = 0;  
  
  for (int i=1; i<N; i++)
    { 
      A[i] = i+1;  
    }

  std::cout << "\n"; //Prime: ";
  
  for (int i=1; i<N; i++)
  {
    if (isprime(A[i]) == true)
      count++;
  }
  std::cout << count;
}

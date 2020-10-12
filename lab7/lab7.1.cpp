#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define N 11

int main(void){
  int i;
  double x[N], y[N];
  x[0] = 0; //при x[0] = 0, все остальные x так же = 0, соотвественно возникает деление на ноль в y[]

  #pragma omp parallel 
  {
    #pragma omp for nowait
      for(i=1; i<N; i++){
        x[i] = x[i-1] * x[i-1]; //проблема гонок, x[i] использует x[i-1]
        y[i] = x[i] / y[i-1]; //проблема гонок, y[i] использует y[i-1]
      }
  }  
  y[0] = x[N-1];

  for(i=1; i<N; i++){
    printf("%g   %g\n", x[i], y[i]);
  }

  return 0;

}
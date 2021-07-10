/*
**  PROGRAM: A simple serial producer/consumer program
**
**  One function generates (i.e. produces) an array of random values.
**  A second functions consumes that array and sums it.
**
**  HISTORY: Written by Tim Mattson, April 2007.
*/
#include <omp.h>
//#ifdef APPLE
//#include <stdlib.h>
//#else
//#include <malloc.h>
//#endif

#include <stdio.h>
#include <stdlib.h>

#define N 1000000

/* Some random number constants from numerical recipies */
#define SEED 2531
#define RAND_MULT 1366
#define RAND_ADD 150889
#define RAND_MOD 714025
int randy = SEED;

/* function to fill an array with random numbers */
void fill_rand(int length, double *a) {
    int i;
    for (i = 0; i < length; i++) {
        randy = (RAND_MULT * randy + RAND_ADD) % RAND_MOD;
        *(a + i) = ((double) randy) / ((double) RAND_MOD);
    }
}

/* function to sum the elements of an array */
double Sum_array(int length, double *a) {
    int i;
    double sum = 0.0;
    for (i = 0; i < length; i++) sum += *(a + i);
    return sum;
}

int main() {
    double *A, sum, runtime;
    A = (double *) malloc(N * sizeof(double));
    int num_threads = 0, flag = 0, tmp_flag = 0;
    runtime = omp_get_wtime();
    #pragma omp parallel default(none) shared(flag, A, sum, tmp_flag)
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                fill_rand(N, A);  // Producer: fill an array of data
                #pragma omp flush
                #pragma omp atomic write
                flag = 1;
                #pragma omp flush(flag)
            }
            #pragma omp section
            {
                while (1) {
                  #pragma omp flush(flag)
                  #pragma omp atomic read
                  tmp_flag = flag;
                  if (tmp_flag == 1) {
                    break;
                  }
                }
                #pragma omp flush
                sum = Sum_array(N, A);  // Consumer: sum the array
            }
        };
    }
    runtime = omp_get_wtime() - runtime;
    free(A);
    double ref = 500060.866846;
    printf(" In %f seconds, The sum is %f(refer %f) \n", runtime, sum, ref);
}

#include <stdio.h>

#define SIZE 100

#define UNIF 1

#define EXPMEAN 10

double uniform(double *);
double expon(double *, float);

int done = 0;

int main() {

  int i,k;
  float emean = EXPMEAN;
  double seed;
  double r;

  done = k = 0;
  seed = 1234567; //set the seed only once, at the start.

  while (!done) {

    k++;
    if (UNIF) {
      r = uniform(&seed);
      if (seed == 1234567) done = 1;

    }
    else {
      r = expon(&seed,emean);
    }
  }
  printf("\n period = %d\n",k);
}
      
      


#include <stdio.h>

#define SIZE 100

#define UNIF 0

#define EXPMEAN 10

double uniform(double *);
double expon(double *, float);


int main() {

  int i;
  float emean = EXPMEAN;
  double seed;
  double r;

  seed = 1234567; //set the seed only once, at the start.

  for (i=0; i < SIZE; i++) {

    if (UNIF) {
      r = uniform(&seed);
    }
    else {
      r = expon(&seed,emean);
    }

    printf("\n Variate #%d    %lf",i,r);
  }
}
      
      
      
    


#ifndef _WARMUP2_H_
#define _WARMUP2_H_

#include <stdio.h>
#include <stdlib.h>
#include "cs402.h"
#include "my402list.h"

typedef struct data {
	int inv_m;
	int P;
	int inv_l;
} data;

int inverse_lambda;	// millisec; positive integer; max=10,000, not for tsfile
int inverse_mu;
int inverse_r;
int inv_l;
int inv_m;
int inv_r;
int B;	// positive integer; max=2,147,483,647
int P;
int num;		
double lambda;	// sec; positive real number
double mu;
double r;
int flag ;	// 2 modes

My402List list_data;

void warmup2_input(int argc, char *argv[]);

#endif

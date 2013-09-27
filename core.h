#ifndef _CORE_H_
#define _CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include "cs402.h"
#include "my402list.h"

typedef struct Q1 {
	int packet_no;
	int inv_m;
	int P;
} Q1;

typedef struct Q2 {
	int packet_no;
	int inv_m;  
} Q2;

typedef struct packet {
	int packet_no;
	double arv_time;
	double enter_Q1;
	double leave_Q1;
	double enter_Q2;
	double begin_S;
	double leave_S;
} packet;

typedef void (*sighandler_t)(int);
sighandler_t sigset(int signo, sighandler_t handler);

struct timeval tim;
double t;
double t0;

pthread_t thread_id[3];
pthread_mutex_t mutex;
pthread_cond_t cond;

#endif

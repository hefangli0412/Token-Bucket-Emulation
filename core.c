#include <stdio.h>	//for printf()
#include <stdlib.h>
#include <sys/time.h>//for gettimeofday()
#include <time.h>	//for nanosleep()
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include "my402list.h"
#include "warmup2.h"
#include "core.h"

int token_all = 0;
int token_count = 0;
int token_cur = 0;
int packet_drp = 0;
int packet_arv = 0;

double arv_time = 0.0;
int arv_num = 0;
double avr_arv_time;

double srv_time = 0.0;
int srv_num = 0;
double avr_srv_time;

int flag_thr1 = 0;
int flag_thr2 = 0;

My402List list_Q1, list_Q2, list_packet;

void msleep(int ms) {
	struct timespec tim2;
	tim2.tv_sec = ms / 1000;
	tim2.tv_nsec = (ms % 1000) * (1000 * 1000);
	nanosleep(&tim2, NULL);
}

void *thr_fn1()
{	
	int k, item_1, item_2, item_3;
	for (k = 0; k < num; k++) 
	{
		/* read data from list */
		
		if (flag != 0) {
			int j;
			My402ListElem *E = My402ListFirst(&list_data);
			for (j = 0; j < k; j++) {
				E = My402ListNext(&list_data, E);
			}
			item_1 = ((data*) (E->obj))->inv_l;
			item_2 = ((data*) (E->obj))->P;
			item_3 = ((data*) (E->obj))->inv_m;			
		} else {
			item_1 = inverse_lambda;
			item_2 = P;
			item_3 = inverse_mu;	
		}


		/* sleep */
		msleep(item_1);	

		/* create Q1 packet */
		Q1 *s;	
		s = (Q1*) malloc(sizeof(Q1));
		if (s != NULL) {
			s->packet_no = k + 1;
			s->P = item_2;
			s->inv_m = item_3;
		}


		packet *s2;
		s2 = (packet*) malloc(sizeof(packet));
		if (s2 != NULL) {
			s2->packet_no = s->packet_no;
			s2->enter_Q1 = 0;
			s2->leave_Q1 = 0;
			s2->enter_Q2 = 0;
			s2->begin_S = 0;
			s2->leave_S = 0;
		}


		/* lock */
		pthread_mutex_lock(&mutex);


		packet_arv++;
		/* create packet packet and record arv_time and add to packet list */

		gettimeofday(&tim, NULL);		
		t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
		if (k == 0) {
			printf("%012.3fms: p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n",
					t - t0, s->packet_no, s->P, t - t0);
			s2->arv_time = t;
			arv_time = arv_time + t - t0;
			arv_num++;
		} else {
			printf("%012.3fms: p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n",
			t - t0, s->packet_no, s->P, 
			t - ((packet*) (My402ListLast(&list_packet)->obj))->arv_time);
			s2->arv_time = t;		
			arv_time = arv_time + t - ((packet*) (My402ListLast(&list_packet)->obj))->arv_time;
			arv_num++;
		}


		My402ListAppend(&list_packet, s2);

		/* enter Q1 */
		My402ListAppend(&list_Q1, s);	

		/* record enter_Q1_time */
		gettimeofday(&tim, NULL);		
		t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
		s2->enter_Q1 = t;
		printf("%012.3fms: p%d enters Q1\n", t - t0, s->packet_no);	

		/* read data from Q1 head */
		Q1* s3 = (Q1*) (My402ListFirst(&list_Q1)->obj);	
		int item_01 = s3->P;
		int item_02 = s3->inv_m;
		int item_03 = s3->packet_no;

		/* find the corresponding packet packet */
		My402ListElem *R = My402ListFirst(&list_packet);
		for (; (R != &list_packet.anchor) ;(R = R->next) ) {
			if ( (((packet*) (R->obj))->packet_no == item_03) != 0 ) break;
		}

		/* drop the inappropriate packet */
		if (item_01 > B) {
			My402ListUnlink(&list_Q1, My402ListFirst(&list_Q1));
			My402ListUnlink(&list_packet, R);
			packet_drp++;
			goto a;
		}

		/* appropriate packet moves from Q1 to Q2 */
		if (item_01 <= token_cur) 
		{
			/* create Q2 packet */
			Q2 *s1;	
			s1 = (Q2*)malloc(sizeof(Q2));
			if (s1 != NULL) {
			s1->inv_m = item_02;
			s1->packet_no = item_03;
			}

			/* drop and record leave_Q1_time */
			My402ListUnlink(&list_Q1, My402ListFirst(&list_Q1));
			token_cur = token_cur - item_01;

			gettimeofday(&tim, NULL);	
			t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
			((packet*) (R->obj))->leave_Q1 = t;
			printf("%012.3fms: p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d tokens\n", 
				t - t0, item_03, t - ((packet*) (R->obj))->enter_Q1, token_cur);

			/* add and record enter_Q2_time */
			My402ListAppend(&list_Q2, s1);

			gettimeofday(&tim, NULL);	
			t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
			((packet*) (R->obj))->enter_Q2 = t;
			printf("%012.3fms: p%d enters Q2\n", t - t0, item_03);

			/* broadcast */
			pthread_cond_signal(&cond);
		}

		/* unlock */
		a: pthread_mutex_unlock(&mutex);	
	}
	//printf("Thread 1 terminated.\n");
	flag_thr1++;
	pthread_testcancel();

	return (void*) 0;
}

void *thr_fn2()
{
	while ( (flag_thr1 == 0) || (My402ListLength(&list_Q1) == 0) == 0)
	{
		/* sleep */
		msleep(inverse_r);

		/* lock */
		pthread_mutex_lock(&mutex);

		/* drop inappropriate token and record */
		if ((token_cur >= B) != 0) {
			token_all++;

			gettimeofday(&tim, NULL);	// token_arrive
			t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
			printf("%012.3fms: token t%d arrives, token bucket now has %d tokens\n", 
					t - t0, token_all, token_cur);
			goto b;
		}

		/* add appropriate token to bucket and record */
		token_cur++;
		token_all++;
		token_count++;

		gettimeofday(&tim, NULL);	// token_arrive
		t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
		printf("%012.3fms: token t%d arrives, token bucket now has %d tokens\n", 
				t - t0, token_all, token_cur);

		/* read data from Q1 head */
		if (My402ListEmpty(&list_Q1)) {
			goto b;
		}
		Q1* tmp_m = (Q1*) (My402ListFirst(&list_Q1)->obj);
		int item_1 = tmp_m->P;
		int item_2 = tmp_m->inv_m;
		int item_3 = tmp_m->packet_no;

		/* find the corresponding packet packet */
		My402ListElem *R = My402ListFirst(&list_packet);
		for (; (R != &list_packet.anchor) ;(R = R->next) ) {
			if ( (((packet*) (R->obj))->packet_no == item_3) != 0 ) break;
		}

		/* appropriate packet moves from Q1 to Q2 */
		if ((token_cur >= item_1) != 0)
		{
			/* create Q2 packet */
			Q2 *s1;	
			s1 = (Q2*)malloc(sizeof(Q2));
			if (s1 != NULL) {
			s1->inv_m = item_2;
			s1->packet_no = item_3;
			}
			

			/* drop and record leave_Q1_time */
			My402ListUnlink(&list_Q1, My402ListFirst(&list_Q1));
			token_cur = token_cur - item_1;	

			gettimeofday(&tim, NULL);	
			t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
			((packet*) (R->obj))->leave_Q1 = t;
			printf("%012.3fms: p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d tokens\n", 
					t - t0, item_3, t - ((packet*) (R->obj))->enter_Q1, token_cur);

			/* add and record enter_Q2_time */
			My402ListAppend(&list_Q2, s1);

			gettimeofday(&tim, NULL);	
			t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
			((packet*) (R->obj))->enter_Q2 = t;
			printf("%012.3fms: p%d enters Q2\n", t - t0, item_3);

			/* broadcast */
			pthread_cond_signal(&cond);
		}
		
		/* unlock */
		b: pthread_mutex_unlock(&mutex);
	}

		flag_thr2++;
		//printf("Thread 2 terminated.\n");
		return (void*) 0;
}

void *thr_fn3()
{
	while ((flag_thr2 == 0) || (My402ListLength(&list_Q2) == 0) == 0) 
	{
		/* lock */
		pthread_mutex_lock(&mutex);

		/* if empty, wait for signal */
		while (My402ListEmpty(&list_Q2)) {
			pthread_cond_wait(&cond, &mutex);
		}

		/* when unblocked */
		/* read data from Q2 head */
		Q2* tmp_m = (Q2*) (My402ListFirst(&list_Q2)->obj);	
		int item_1 = tmp_m->inv_m;
		int item_2 = tmp_m->packet_no;

		/* find the corresponding packet packet */
		My402ListElem *R = My402ListFirst(&list_packet);
		for (; (R != &list_packet.anchor) ;(R = R->next) ) {
			if ( (((packet*) (R->obj))->packet_no == item_2) != 0 ) break;
		}

		/* leave Q2 / begin S and record */
		My402ListUnlink(&list_Q2, My402ListFirst(&list_Q2));

		gettimeofday(&tim, NULL);	
		t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
		((packet*) (R->obj))->begin_S = t;
		printf("%012.3fms: p%d begin service at S, time in Q2 = %.3fms\n", 
				t - t0, item_2, t - ((packet*) (R->obj))->enter_Q2);

		/* unlock */
		pthread_mutex_unlock(&mutex);

		/* sleep */
		msleep(item_1);	

		/* lock */
		pthread_mutex_lock(&mutex);

		/* leave S and record */
		gettimeofday(&tim, NULL);	
		t = tim.tv_sec * 1000.0 + (tim.tv_usec / 1000.0);
		((packet*) (R->obj))->leave_S = t;
		srv_time = srv_time + t - ((packet*) (R->obj))->begin_S;
		srv_num++;
		printf("%012.3fms: p%d departs from S, service time = %.3fms, time in system = %.3fms\n", 
				t - t0, item_2, t - ((packet*) (R->obj))->begin_S, t - ((packet*) (R->obj))->arv_time);

		/* unlock */
		pthread_mutex_unlock(&mutex);

	}

	return (void*) 0;
}

void calc()
{
	/*
	printf("list_packet: %d\n", My402ListLength(&list_packet));
	printf("list_Q1: %d\n", My402ListLength(&list_Q1));
	printf("list_Q2: %d\n", My402ListLength(&list_Q2));
	printf("packet_arv: %d\n", packet_arv);
	printf("srv_num: %d\n", srv_num);
	*/

	arv_time = arv_time * 0.001;
	srv_time = srv_time * 0.001;
	printf("\nStatistics:\n");
	avr_arv_time = arv_time / (double)arv_num;
	printf("   average packet inter-arrival time = %.6gsec\n", avr_arv_time);
	avr_srv_time = srv_time / (double)srv_num;
	printf("   average packet service time = %.6gsec\n\n", avr_srv_time);

	double Q1_time_all = 0;
	double Q2_time_all = 0;
	double S_time_all = 0;
	double sys_time_all = 0; 
	double tmp = 0;

	My402ListElem *R = My402ListFirst(&list_packet);
	for (; (R != &list_packet.anchor) ;(R = R->next) ) {
		Q1_time_all = Q1_time_all + ((packet*) (R->obj))->leave_Q1 - ((packet*) (R->obj))->enter_Q1; 	
	
	}
	Q1_time_all = Q1_time_all * 0.001;

	R = My402ListFirst(&list_packet);
	for (; (R != &list_packet.anchor) ;(R = R->next) ) {
		Q2_time_all = Q2_time_all + ((packet*) (R->obj))->begin_S - ((packet*) (R->obj))->enter_Q2; 
	
	}
	Q2_time_all = Q2_time_all * 0.001;

	R = My402ListFirst(&list_packet);
	for (; (R != &list_packet.anchor) ;(R = R->next) ) {
		S_time_all = S_time_all + ((packet*) (R->obj))->leave_S - ((packet*) (R->obj))->begin_S; 
		//printf("S_time_all: %.3f\n", S_time_all);

	}
	S_time_all = S_time_all * 0.001;

	R = My402ListFirst(&list_packet);
	for (; (R != &list_packet.anchor) ;(R = R->next) ) {
		sys_time_all = sys_time_all + ((packet*) (R->obj))->leave_S - ((packet*) (R->obj))->arv_time; 
		//printf("sys_time_all: %.3f\n", sys_time_all);
		tmp = tmp + (((packet*) (R->obj))->leave_S - 
			((packet*) (R->obj))->arv_time) * (((packet*) (R->obj))->leave_S - ((packet*) (R->obj))->arv_time);
	}
	sys_time_all = sys_time_all * 0.001;
	tmp = tmp * 0.001 * 0.001;
	double total_time = (t00 - t0) * 0.001;

	double avr_Q1_num = Q1_time_all / total_time;
	printf("   average number of packets in Q1 = %.6g\n", avr_Q1_num);
	double avr_Q2_num = Q2_time_all / total_time;
	printf("   average number of packets in Q2 = %.6g\n", avr_Q2_num);
	double avr_srv_num = S_time_all / total_time;
	printf("   average number of packets at S = %.6g\n\n", avr_srv_num);
	double avr_sys_time = sys_time_all / srv_num;
	printf("   average time a packet spent in system = %.6gsec\n", avr_sys_time);
	double sd_sys_time = sqrt(tmp / (double)srv_num - avr_sys_time * avr_sys_time);
	printf("   standard deviation for time spent in system = %.6g\n\n", sd_sys_time);

	double tk_drp_prb = 1 - (double)token_count / (double)token_all;
	printf("   token drop probability = %.6g\n", tk_drp_prb);
	double pk_drp_prb = (double)packet_drp / (double)packet_arv;
	printf("   packet drop probability = %.6g\n", pk_drp_prb);

	/*
	R = My402ListFirst(&list_packet);
	for (; (R != &list_packet.anchor) ;(R = R->next) ) {
		printf("list_packet.obj->arv_time: %.3f\n", ((packet*)(R->obj))->arv_time - t0); 	
		printf("list_packet.obj->enter_Q1: %.3f\n", ((packet*)(R->obj))->enter_Q1 - t0);
		printf("list_packet.obj->leave_Q1: %.3f\n", ((packet*)(R->obj))->leave_Q1 - t0);
		printf("list_packet.obj->enter_Q2: %.3f\n", ((packet*)(R->obj))->enter_Q2 - t0);
		printf("list_packet.obj->begin_S: %.3f\n", ((packet*)(R->obj))->begin_S - t0);
		printf("list_packet.obj->leave_S: %.3f\n", ((packet*)(R->obj))->leave_S - t0);
	}
	*/
}

void handler(int signo)
{
	//printf("Opps.\n");
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_cancel(thread_id[0]);
	pthread_cancel(thread_id[1]);
	flag_thr2++;
}

int main(int argc, char *argv[])
{
	int i;

	warmup2_input(argc, argv);

	memset(&list_Q1, 0, sizeof(My402List));
	memset(&list_Q2, 0, sizeof(My402List));
	memset(&list_packet, 0, sizeof(My402List));
	(void) My402ListInit(&list_Q1);
	(void) My402ListInit(&list_Q2);
	(void) My402ListInit(&list_packet);

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	int s1, s2, s3;
	s1 = pthread_create(&thread_id[0], NULL, thr_fn1, 0);
	if (s1 != 0) {
		fprintf(stderr, "error: pthr_id_0_create.\n");
		return EXIT_FAILURE;
	}
	s2 = pthread_create(&thread_id[1], NULL, thr_fn2, 0);
	if (s2 != 0) {
		fprintf(stderr, "error: pthr_id_1_create.\n");
		return EXIT_FAILURE;
	}
	s3 = pthread_create(&thread_id[2], NULL, thr_fn3, 0);
	if (s3 != 0) {
		fprintf(stderr, "error: pthr_id_2_create.\n");
		return EXIT_FAILURE;
	}

	gettimeofday(&tim, NULL);
	t0 = tim.tv_sec * 1000.0 + (tim.tv_usec/1000.0);
	printf("%012.3fms: emulation begins\n", t0 - t0);	

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	sigset(SIGINT, handler);

	for (i = 0; i < 3; i++) {
		pthread_join(thread_id[i], NULL);
	}
	//printf("Thread 3 terminated.\n");

	gettimeofday(&tim, NULL);
	t00 = tim.tv_sec * 1000.0 + (tim.tv_usec/1000.0);
	//printf("%012.3fms: emulation endns\n", t00 - t0);

	calc();

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	return 0;
}

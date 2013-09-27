#include <stdio.h>
#include <stdlib.h>		// for exit()
#include <string.h>		// for strcmp()
#include <ctype.h>		// for isspace()
#include "my402list.h"
#include "warmup2.h"

#define R_OPTION "-r"	
#define B_OPTION "-B"
#define LAMBDA_OPTION "-lambda"
#define MU_OPTION "-mu"
#define P_OPTION "-P"
#define NUM_OPTION "-n"
#define T_OPTION "-t"

/*
warmup2_input() -- Input part contains 2 modes. When in use, please check flag.
*/
void warmup2_input(int argc, char *argv[])
{
	FILE* fp;	//tsfile

	int i;
	flag = 0;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int flag4 = 0;
	int flag5 = 0;
	int flag6 = 0;

	// First find if there are inverse_r and B inputs.
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], R_OPTION) == 0)
		{
			r = atof(argv[i + 1]);
			if (r <= 0) 
			{
				printf("The value of r must be positive.");
				exit(1);
			} 
			else 
			{
				inverse_r = 1000.0 / r;	//millisec
				if (inverse_r > 10000) {
					inverse_r = 10000;
				}
			}

			flag3++;
		}
		else if (strcmp(argv[i], B_OPTION) == 0) 
		{
			B = atoi(argv[i + 1]);
			if ((B <= 0 && B > 2147483647) != 0) 
			{ 
				printf("The value of B is unacceptable.");
				exit(1);
			}

			flag4++;
		}
	}

	// Second find if there is tsfile input.
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], T_OPTION) == 0)
		{
			i++;
			fp = fopen(argv[i], "r");
			if (fp == 0) {
				printf("Could not open file \"%s\"\n", argv[i]);
				exit(1);
			}

			memset(&list_data, 0, sizeof(My402List));
			(void) My402ListInit(&list_data);		//Initialization of List

		/*
		tsfile_read() -- one process of mode Detem
		*/
			char buf[50];

			fgets(buf, sizeof(buf), fp);	
			num = atoi(&buf[0]);
			if ((num <= 0 && num > 2147483647) != 0) { 
				printf("The value of num is unacceptable.");
				exit(1);
			}

			while (!feof(fp))
			{
				int j = 0;
				data* myobj = (data*) malloc(sizeof(data));
				fgets(buf, sizeof(buf), fp);

				myobj->inv_l = atof(&buf[j]);

				while (isspace((int) buf[j]) == 0) {	
					j++;
				}
				j++;
				myobj->P = atoi(&buf[j]);
				if ((P <= 0 && P > 2147483647) != 0) { 
					printf("The value of P is unacceptable.");
					exit(1);
				}

				while (isspace((int) buf[j]) == 0) {
					j++;
				}
				j++;
				myobj->inv_m = atof(&buf[j]);	

				My402ListAppend(&list_data, myobj);
			}

			flag++;
		}
	}

	// Third if there is no tsfile, find 4 other constants.
	if (flag == 0)
	{
		for (i = 0; i < argc; i++)
		{
			if (strcmp(argv[i], LAMBDA_OPTION) == 0)
			{
				lambda = atof(argv[i + 1]);
				if (lambda <= 0)
				{
					printf("The value of lambda must be positive.");
					exit(1);
				} 
				else 
				{
					inverse_lambda = 1000.0 / r;
					if (inverse_lambda > 10000) {
						inverse_lambda = 10000;
					}
				}
				flag1++;
			} 
			else if (strcmp(argv[i], MU_OPTION) == 0) 
			{
				mu = atof(argv[i + 1]);
				if (mu <= 0)
				{
					printf("The value of mu must be positive.");
					exit(1);
				} 
				else 
				{
					inverse_mu = 1000.0 / r;
					if (inverse_mu > 10000) {
						inverse_mu = 10000;
					}
				}
				flag2++;
			}
			else if (strcmp(argv[i], P_OPTION) == 0)	
			{
				P = atoi(argv[i + 1]);
				if ((P <= 0 && P > 2147483647) != 0) { 
					printf("The value of P is unacceptable.");
					exit(1);
				}
				flag5++;
			}
			else if (strcmp(argv[i], NUM_OPTION) == 0)
			{
				num = atoi(argv[i + 1]);
				if ((num <= 0 && num > 2147483647) != 0) {
					printf("The value of num is unacceptable.");
					exit(1);
				}
				flag6++;
			}

		}

		if (flag6 == 0) {
		num = 7;
		}
	}

	// If any info missing, gibe the default value for all 6 variables.
	if (flag1 == 0) {
		lambda = 0.5;
		inverse_lambda = 2000;	//millisec
	}
	if (flag2 == 0) {
		mu = 0.35;
		inverse_mu = (int)((20.0*1000.0)/7.0);	//millisec
	}
	if (flag3 == 0) {
		r = 1.5;
		inverse_r = (int)((2.0*1000.0)/3.0);	//millisec
	}
	if (flag4 == 0) {
		B = 10;
	}	
	if (flag5 == 0)	{
		P = 3;
	}


	// Print all the emulation paramters.
	if (flag == 0)
	{
		printf("Emulation Parameters:\n");
		printf("    lambda = %g\n", lambda);
		printf("    mu = %g\n", mu);
		printf("    r = %g\n", r);
		printf("    B = %d\n", B);
		printf("    P= %d\n", P);
		printf("    number to arrive = %d\n\n", num);		
	}
	else
	{
		printf("Emulation Parameters:\n");
		printf("    r = %g\n", r);
		printf("    B = %d\n", B);
		printf("    tsfile = %s\n\n", (char*)fp);	
	}

	/* data format convertion */

}

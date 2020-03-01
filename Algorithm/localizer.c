 /* main.c
 *
 *  Created on: Feb 28, 2020
 *      Author: Xinyu Cao
 */

// all times are in ms, all distances are in cm

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define SENSOR0 0
#define SENSOR1 1
#define SENSOR2 2
#define SENSOR3 3
#define SOUND_SPEED (343*100/1000)  // uint: cm/ms
#define M 100 	// height of the board is 100 cm
#define N 210	// width of the board is 210 cm

/*                       board with four sensors                    */
/*         0                                         3                */
/*      --- *****************************************               */
/*       |  *                                       *                */
/*       |  *    *                                  *                */
/*       |  *                                       *                */
/*       m  *                                       *                */
/*       |  *                                       *                */
/*       |  *                                       *                */
/*       |  *                                       *                */
/*       |  *                                       *                */
/*       |  *                                       *                */
/*      --- *****************************************               */
/*         1                                         2                */
/*         |----------------------n-----------------|                */

//given position x and y relative to sensor 0, return absolute time of arrivals from sound source to each sensor in ms 
double* getTimesFromPos(double x, double y);

//absolute time of arrivals times[4], return the calculated coordinates of the sound source in a double[2]
double* getPosFromTimes(double *times);

int time();

void main(){
   	double *times;
   	double *pos;
   	double x;
   	double y;

   	printf("\n");

   	int totalCnt = 0;
   	int correctCnt = 0;

   	/* 									thorough testing 					*/

   	for(int i = 1; i < M; i+=1){
   		for(int j = 1; j < N; j+=1){
   			totalCnt++;
   			x = i;
		   	y = j;
		   	times = getTimesFromPos(x,y);
		   	pos = getPosFromTimes(times);
		   	if(x - pos[0] < 0.1 && y - pos[1] < 0.1){
		   		printf("expected: (%f, %f), calculated: (%f, %f) --- correct\n", x, y, pos[0], pos[1]);
		   		correctCnt++;
		   	}
		   	else
		   		printf("expected: (%f, %f), calculated: (%f, %f) --- wrong\n", x, y, pos[0], pos[1]);
   		}
   	}

   	printf("correct/total: %d/%d\n", correctCnt, totalCnt);

/* 									single testing 							*/

	// x = 20;
 //   	y = 200;
 //   	times = getTimesFromPos(x,y);
 //   	printf("test case: (%f,%f)\n",x,y);
 //   	printf("Simulated times\n");
   	
 //   	printf("travel time to sensor 0 is %lf ms\n", times[SENSOR0]);
 //   	printf("travel time to sensor 1 is %lf ms\n", times[SENSOR1]);
 //   	printf("travel time to sensor 2 is %lf ms\n", times[SENSOR2]);
 //   	printf("travel time to sensor 3 is %lf ms\n", times[SENSOR3]);

 //   	pos = getPosFromTimes(times);
 //   	printf("sound source position: x= %lf, y=%lf\n", pos[0], pos[1]);

   	
}

double* getPosFromTimes(double *times){

	double a; 	//DOA between sensor 1 and sensor 0
	double b; 	//DOA between sensor 2 and sensor 0
	double c; 	//DOA between sensor 3 and sensor 0

	// A*r^2 + B*r + C = 0 (*)

	double A;	//coefficient of the power 2 term of r
	double B;	//coefficient of the power 1 term of r
	double C;	//coefficient of the power 0 term of r
	double discriminant;	//the discriminant of the equation (*) 
	double r1;
	double r2;
	double r;
	static double result[2];

	a = (times[1] - times[0]) * SOUND_SPEED; 	
	b = (times[2] - times[0]) * SOUND_SPEED; 	
	c = (times[3] - times[0]) * SOUND_SPEED; 

	printf("a = %lf, b = %lf,c = %lf\n", a,b,c );

	A = ((a*a) / (M*M)) + ((c*c) / (N*N)) - 1;
	B = - ((a*M*M - a*a*a) / (M*M) + (c*N*N - c*c*c) / (N*N));
	C = pow((M*M - a*a) / (2*M), 2) + pow((N*N - c*c) / (2*N), 2);

	discriminant = B*B - 4*A*C;

	//return error {-1,-1} if discriminant is negative, i.e. there's no real solutions for r
	if(discriminant < 0){
		static double errorPos[2];
		errorPos[0] = -1;
		errorPos[1] = -1;
		return errorPos;
	}

	r1 = (-B + sqrt(discriminant)) / (2*A);
	r2 = (-B - sqrt(discriminant)) / (2*A); 

	int max_r = sqrt(M*M+N*N); //the diagonal length is the largest a valid r can be

	if(r1 > 0 && r1 < max_r){
		r = r1;
		
		result[0] = (M*M - a*a - 2*a*r) / (2 * M);
		result[1] = (N*N - c*c - 2*c*r) / (2 * N);

		printf("r1 = %f, x = %lf, y = %lf\n", r1, result[0], result[1]);
	}
	if(r2 > 0 && r2 < max_r){
		r = r2;
		//printf("r2 = %f\n", r2);
		result[0] = (M*M - a*a - 2*a*r) / (2 * M);
		result[1] = (N*N - c*c - 2*c*r) / (2 * N);

		printf("r2 = %f, x = %lf, y = %lf\n", r2, result[0], result[1]);
	}

   	return result;

}

//given position, return absolute time of arrivals from sound source to each sensor in ms 
double* getTimesFromPos(double x, double y){
	double d[4];

	d[0] = sqrt(x*x+y*y); 	//SS-S0
	d[1] = sqrt((M-x)*(M-x)+y*y);	//SS-S1
	d[2] = sqrt((M-x)*(M-x)+(N-y)*(N-y));	//SS-S2
	d[3] = sqrt(x*x+(N-y)*(N-y));	//SS-S3

	static double times[4];

	double randomStartTime; //simulate the random start time of the clock
	srand(time(NULL));
	randomStartTime = (double)rand();

	for(int i = 0; i < 4; i++)
		times[i] = d[i] / SOUND_SPEED + randomStartTime;

	return times;
}


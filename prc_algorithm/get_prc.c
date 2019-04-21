#include<stdio.h>
#include<iostream>
//#include<Eigen/Dense>
#include"../Eigen/Dense"
#include"../parameters.h"
#define Pi 3.14159265358979

using namespace std;
using namespace Eigen;

double PRC(double *prc, double x){
	double res = prc[1];
	for(int p = 0; p < Nfourier; ++p){
		res += prc[1+2*p+1]*sin((p+1)*x);
		res += prc[1+2*p+2]*cos((p+1)*x);
	}
	return res;
}

int main(void){
	
	FILE *f;
	//count the pulses
	int st_heart = 0;
	f = fopen("signal_thresholding/events.txt","rt");
	double t, s;
	while(fscanf(f, "%lf\n", &t) != EOF){
		st_heart++;
	}
	fclose(f);
	int st_resp = 0;
	f = fopen("data/driving.txt","rt");
	while(fscanf(f, "%lf, %lf\n", &t, &s) != EOF){
		st_resp++;
	}
	fclose(f);
	//reading it in
	double *heart_spikes = new double[st_heart];
	st_heart = 0;
	f = fopen("signal_thresholding/events.txt","rt");
	while(fscanf(f, "%lf\n", &t) != EOF){
		heart_spikes[st_heart] = t;
		st_heart++;
	}
	fclose(f);
	double **resp_signal = new double*[st_resp];
	for(int i = 0; i < st_resp; ++i) resp_signal[i] = new double[2];
	st_resp = 0;
	f = fopen("data/driving.txt","rt");
	while(fscanf(f, "%lf %lf\n", &t, &s) != EOF){
		resp_signal[st_resp][0] = t;
		resp_signal[st_resp][1] = s;
		st_resp++;
	}
	fclose(f);
	//make intervals
	double ***intervals = new double**[st_heart-1];
	st_resp = 0;
	f = fopen("data/driving.txt","rt");
	t = 0;
	while(t < heart_spikes[0]) fscanf(f, "%lf %lf\n", &t, &s); //first data points before the first heart beat
	double **stimuli_buffer = new double*[1000000];
	for(int i = 0; i < 1000000; ++i) stimuli_buffer[i] = new double[2]; //define stimuli buffer
	for(int in = 0; in < st_heart-1-1; ++in){ //loop over intervals
		int stimuli_counter = 0;
		while(t < heart_spikes[in+1]){
			stimuli_buffer[stimuli_counter][0] = t;
			stimuli_buffer[stimuli_counter][1] = s;
			fscanf(f, "%lf %lf\n", &t, &s);
			stimuli_counter++;
		} 
		intervals[in] = new double*[stimuli_counter+1]; //+1 because intervals[in][0][0] will be stimuli_counter, and intervals[in][0][1] will be T
		for(int i = 0; i < stimuli_counter+1; ++i) intervals[in][i] = new double[2];
		intervals[in][0][0] = stimuli_counter; 
		intervals[in][0][1] = heart_spikes[in+1]-heart_spikes[in];
		for(int i = 1; i < stimuli_counter+1; ++i){
			intervals[in][i][0] = stimuli_buffer[i-1][0]-heart_spikes[in];
			intervals[in][i][1] = stimuli_buffer[i-1][1];
			if(intervals[in][i][0] < 0) printf("trouble, in = %d\n", in);
		}
	}
	//lets make the matrix
	MatrixXf A(st_heart-1, 1+1+2*Nfourier);
	for(int in = 0; in < st_heart-1-1; ++in){ //loop over intervals
		A(in,0) = intervals[in][0][1];
		A(in,1) = 0;
		for(int i = 1; i < intervals[in][0][0]+1; ++i){
			//A(in,1) += intervals[in][i][1]; //stimuli weights (integrating respiration signal)
			if(i != 1) A(in,1) += intervals[in][i][1]*(intervals[in][i][0]-intervals[in][i-1][0]); //stimuli weights (integrating respiration signal)
			else A(in,1) += intervals[in][i][1]*intervals[in][i][0];
		}
		for(int f = 0; f < Nfourier; ++f){
			double sums = 0;
			double sumc = 0;
			for(int i = 1; i < intervals[in][0][0]+1; ++i){
				double phase = intervals[in][i][0]/intervals[in][0][1]*2*Pi; //t/T*2*Pi
				if(i != 1){
					sums += sin((f+1)*phase)*intervals[in][i][1]*(intervals[in][i][0]-intervals[in][i-1][0]);
					sumc += cos((f+1)*phase)*intervals[in][i][1]*(intervals[in][i][0]-intervals[in][i-1][0]);
				}
				else{
					sums += sin((f+1)*phase)*intervals[in][i][1]*intervals[in][i][0];
					sumc += cos((f+1)*phase)*intervals[in][i][1]*intervals[in][i][0];
				}
				
			}
			A(in,2+2*f) = sums;
			A(in,2+2*f+1) = sumc;
		}
	}
	//and now the result
	VectorXf b(st_heart-1);
	for(int i = 0; i < st_heart-1; ++i) b(i) = 2*Pi;
	MatrixXf ATAA = A.transpose()*A;
	ATAA = ATAA.inverse();
	ATAA = ATAA*A.transpose();
	VectorXf r(1+1+2*Nfourier);
	r = ATAA*b;
	//file print
	f = fopen("prc_algorithm/PRC.txt","wt");
	for(int i = 0; i < 1+1+2*Nfourier; ++i){
		fprintf(f,"%lf\n", r(i));
	}
	fclose(f);
	//file print (in phase)
	double *prc_harmonics = new double[1+2*Nfourier];
	for(int i = 0; i < 1+2*Nfourier; ++i){
		prc_harmonics[i] = r(i);
	}
	f = fopen("prc_algorithm/PRC_ph.txt","wt");
	for(double ph = 0.5*(2*Pi)/100; ph < 2*Pi; ph += (2*Pi)/100){
		fprintf(f,"%lf %lf\n", ph, PRC(prc_harmonics,ph));
	}
	fclose(f);

	return 0;
	
}


#include<stdio.h>
#include<math.h>
#include"../parameters.h"
#define Pi 3.14159265358979

int main(void){
	
	//count the pulses
	FILE *f;
	int st_heart = 0;
	f = fopen("signal_thresholding/events.txt","rt");
	double t, s;
	while(fscanf(f, "%lf\n", &t) != EOF){
		st_heart++;
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
	//make intervals
	double ***intervals = new double**[st_heart-1];
	int st_resp = 0;
	f = fopen("data/driving.txt","rt");
	t = 0;
	while(t < heart_spikes[0]) fscanf(f, "%lf %lf\n", &t, &s); //first data points before the first event
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
		}
	}
	//getting the average frequency
	double w = 0;
	int w_cnt = 0;
	for(int in = 0; in < st_heart-1-1; ++in){ //loop over intervals
		w += 1/intervals[in][0][1];
		w_cnt++;
	}
	w /= w_cnt;
	w *= 2*Pi;
	//psi-2pi std
	double psi_std = 0;
	int psi_counter = 0;
	for(int in = 0; in < st_heart-1-1; ++in){ //loop over intervals
		double T = intervals[in][0][1];
		psi_std += pow(w*T-2*Pi,2);
		psi_counter++;
	}
	psi_std /= psi_counter;

	f = fopen("error/ERR_psi0.txt","wt");
	fprintf(f,"%.16lf\n", sqrt(psi_std));
	fclose(f);
	
	return 0;
	
}



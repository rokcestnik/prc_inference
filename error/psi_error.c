#include<stdio.h>
#include<math.h>
#include"../parameters.h"
#define Pi 3.14159265358979

double PRC(double *prc, double x){
	double res = prc[1];
	for(int p = 0; p < Nfourier; ++p){
		res += prc[1+2*p+1]*sin((p+1)*x);
		res += prc[1+2*p+2]*cos((p+1)*x);
	}
	return res;
}

int main(void){
	
	//first just read the reconstructed prc
	double *prc = new double[1+1+2*Nfourier];
	FILE *f;
	f = fopen("prc_algorithm/PRC.txt","rt");
	for(int p = 0; p < 1+1+2*Nfourier; ++p){
		fscanf(f, "%lf\n", &(prc[p]));
	}
	fclose(f);
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
	while(fscanf(f, "%lf %lf\n", &t, &s) != EOF){
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
	//lets determine dt... (the time increment has to be constant)
	double dt = resp_signal[2][0]-resp_signal[1][0];
	//make intervals
	double ***intervals = new double**[st_heart-1];
	st_resp = 0;
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
	//psi-2pi std
	double psi_std = 0;
	int psi_counter = 0;
	for(int in = 0; in < st_heart-1-1; ++in){ //loop over intervals
		int hms = intervals[in][0][0]; //how many signals
		double *phasenew = new double[hms+1]; //phase over all times that we have signal
		phasenew[1] = intervals[in][1][0]*prc[0];
		for(int i = 2; i < hms+1; ++i){
			phasenew[i] = (phasenew[i-1]+(prc[0]+PRC(prc,phasenew[i-1])*(intervals[in][i][1]+intervals[in][i-1][1])/2)*dt); 
		}
		double phase_at_end = phasenew[hms]+(intervals[in][0][1]-intervals[in][hms][0])*prc[0];
		//add psi
		psi_std += pow(phase_at_end-2*Pi,2);
		psi_counter++;
	}
	psi_std /= psi_counter;

	f = fopen("error/ERR_psi.txt","wt");
	fprintf(f,"%.16lf\n", sqrt(psi_std));
	fclose(f);
	
	return 0;
	
}



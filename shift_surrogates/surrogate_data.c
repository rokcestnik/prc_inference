#include<stdio.h>
#include"../parameters.h"

int main(int argc, char *argv[]){
	
	int event_counter = 0;
	int timestep_counter = 0;
	
	//determine minimum maximum
	double min = 1000000;
	double max = -1000000;
	double t,s;
	FILE *f;
	f = fopen("../data/signal.txt","rt");
	while(fscanf(f,"%lf %lf\n", &t, &s) != EOF){
		if(max < s) max = s;
		if(min > s) min = s;
	}
	rewind(f);
	FILE *fw;
	fw = fopen("../data/processed_data/events.txt","wt");
	//now determine events
	double spikestart = 0;
	double sp; //s previous
	double tp; //t previous
	fscanf(f,"%lf %lf\n", &tp, &sp);
	while(fscanf(f,"%lf %lf\n", &t, &s) != EOF){
		if(s > min+(max-min)*event_thr && sp < min+(max-min)*event_thr){
			spikestart = tp + (min+(max-min)*event_thr - sp)/(s-sp)*(t-tp);
			event_counter++;
		}
		else if(s < min+(max-min)*event_thr && sp > min+(max-min)*event_thr && spikestart != 0){ //spikestart != 0 just so that the signal doesn't start with a spike
			double spikestop = tp + (min+(max-min)*event_thr - sp)/(s-sp)*(t-tp);
			//fprintf(fw, "%lf\n", (spikestart+spikestop)/2); //for midpoint between pulse start and end
			if(crossing_inclination == 0) fprintf(fw, "%lf\n", spikestart); //for pulse start
			if(crossing_inclination) fprintf(fw, "%lf\n", spikestop); //for pulse end
		}
		sp = s;
		tp = t;
		timestep_counter++;
	}
	fclose(fw);
	fclose(f);
	
	//and now the surrogates
	int shift = int(3.14159*double(timestep_counter)/double(event_counter));
	double *ts = new double[timestep_counter];
	double *ss = new double[timestep_counter];
	f = fopen("../data/driving.txt","rt");
	for(int i = 0; i < timestep_counter; ++i){
		fscanf(f,"%lf %lf\n", &(ts[i]), &(ss[i]));
	}
	fclose(f);
	for(int sur = 0; sur < 10; ++sur){
		char filename[100];
		sprintf(filename, "surrogate_drivings/surr%d.txt", sur);	
		fw = fopen(filename,"wt");
		for(int i = 0; i < timestep_counter; ++i){
			fprintf(fw,"%lf %lf\n", ts[i], ss[(i+(sur+1)*shift)%timestep_counter]);
		}
		fclose(fw);
	}
	
	return 0;
	
}


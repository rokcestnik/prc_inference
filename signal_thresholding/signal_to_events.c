#include<stdio.h>
#include"../parameters.h"

int main(int argc, char *argv[]){
	
	int event_counter = 0;
	
	//determine minimum maximum
	double min,max;
	double t,s;
	FILE *f;
	f = fopen(argv[1],"rt");
	while(fscanf(f,"%lf %lf\n", &t, &s) != EOF){
		if(max < s) max = s;
		if(min > s) min = s;
	}
	rewind(f);
	FILE *fw;
	fw = fopen("signal_thresholding/events.txt","wt");
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
	}
	fclose(fw);
	fclose(f);
	
	printf("%d intervals\n", event_counter-1);
	
	return 0;
	
}


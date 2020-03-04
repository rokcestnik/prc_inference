
//event detection
double event_thr = 0.7; //this is a relative threshold, i.e. the actual threshold value is: signal_min + event_thr*(signal_max-signal_min)
bool crossing_inclination = 0; //0 for crossing from below, 1 for crossing from above
//number of fourier terms
int Nfourier = 5;

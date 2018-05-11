#ifndef FILTER_H_
#define FILTER_H_

#define SIZE 1000

#include <math.h>

void threshold(float data[SIZE], float maxDiff) {
	int i;
	
	for (i = 1; i < SIZE; i++) {
		if (data[i] - data[i-1] > maxDiff || data[i] - data[i-1] < -maxDiff) {
			data[i] = data[i-1];
		}
	}
}

void dc(float data[SIZE], float alpha) {
	int i;
	float w[SIZE];

	for (i = 1; i < SIZE; i++) {
		w[i] = (float)data[i] + alpha*w[i-1];
		data[i] = w[i] - w[i-1];
	}
}

void meanMedian(float data[SIZE], const int window) {
	float values[20];
	float sum = 0;
	int index = 0;
	int count = 0;
	int i;

	for (i = 0; i < window; i++) {
	    values[i] = 0;
	}

	for (i = 0; i < SIZE; i++) {
		float avg;

		sum -= values[index];
		values[index] = data[i];
		sum += values[index];

		index++;
		index = index % window;

		if (count < window) {
			count++;
		}

		avg = sum / (float)count;

		data[i] = avg - data[i];
	}
}

void butterworth(float data[SIZE]) {
	float v[2];
	int i;
	
	for (i = 0; i < SIZE; i++) {
		v[0] = v[1];
		// constants for 50 - 220 BPM, http://www.schwietering.com/jayduino/filtuino/
		v[1] = (2.452372752527856026e-1 * data[i]) + (0.50952544949442879485 * v[0]);

		data[i] = v[0] + v[1];
	}
}

float max(float data[SIZE]) {
	int i;
	float m;
	
	m = data[100];
	
	// offset to the right by 100 after dc filter
	for (i = 100; i < SIZE; i++) {
		if (data[i] > m) {
			m = data[i];
		}
	}
	
	return m;
}

float heartrate(float data[SIZE]) {
	bool peak;
	int beats;
	int i;
	float threshold;
	
	peak = false;
	beats = 0;
	threshold = max(data) / 3.0; // multiplier (1/3) can be tweaked

	// offset to the right by 100 after dc filter
	for (i = 100; i < SIZE; i++) {
		if (!peak && data[i] > threshold) {
			peak = true;
			beats++;
		}

		if (peak && data[i] <= threshold) {
			peak = false;
		}
	}

	// offset to the right by 100 after dc filter => 9 seconds instead of 10
	return (float)beats * 6000.0 / (float)(SIZE - 100);
}

float spo2(float dataIR[SIZE], float dataR[SIZE]) {
	float irSquareSum;
	float rSquareSum;
	float rmsRatio;
	int i;
	
	irSquareSum = 0.0;
	rSquareSum = 0.0;
	rmsRatio = 0.0;
	
	// offset to the right by 100 after dc filter
	for (i = 100; i < SIZE; i++) {
		irSquareSum += dataIR[i] * dataIR[i];
		rSquareSum += dataR[i] * dataR[i];
	}

	rmsRatio = log(sqrt(rSquareSum / (SIZE - 100))) / log(sqrt(irSquareSum / (SIZE - 100)));
	
	//include wavelengths into formula
	//rmsRatio = (log(sqrt(rSquareSum / samplesRecorded)) * 660.0) / (log(sqrt(irSquareSum / samplesRecorded)) * 880.0);
	
	//constants require calibration
	return 115.0 - 18.0 * rmsRatio;
}

#endif

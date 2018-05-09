#ifndef FILTER_H_
#define FILTER_H_

#define SIZE 1000

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
		v[1] = (2.452372752527856026e-1 * data[i]) + (0.50952544949442879485 * v[0]);

		data[i] = v[0] + v[1];
	}
}

float heartrate(float data[SIZE]) {
	bool peak;
	int beats;
	int i;

	for (i = 0; i < SIZE; i++) {
		if (!peak && data[i] > 450.0) {
			peak = true;
			beats++;
		}

		if (peak && data[i] <= 450.0) {
			peak = false;
		}
	}

	return (float)beats * 6000.0 / (float)SIZE;
}

#endif

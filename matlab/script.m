clear;
close all;
clc;

BPM = 210;
LED = 24;
factor = 2;

x = 1:1:2000;
figure;

y = signal(x, BPM, LED);
%subplot(4, 1, 1);
plot(x, y); %xlim([100 2000]);

y = dc(y, 0.95);
%subplot(4, 1, 2);
figure;
plot(x, y); %xlim([100 2000]);

y = meanMedian(y, 15);
%subplot(4, 1, 3);
figure;
plot(x, y); xlim([100 2000]);

y = butterworth(y);
%subplot(4, 1, 4);
figure;
plot(x, y); xlim([100 2000]);

hold on;
t = threshold(y, factor);
plot(x, t); xlim([100 2000]);
hold off;

heartrate(y, factor)
heartrate2(y, factor)

function y = signal(x, BPM, LED)
    SNR = 55 + 120/LED;
    %SNR = 60;
    a = 20.83*LED;
    b = 3300/BPM;
    c = 9900/BPM;
    
    %y = 16.67*LED*sin(x*BPM/(310*pi)) + 20.83*LED*cos(x/(200*pi)) + 1770.83*LED;
    y = a*exp(-(mod(x+30, 6000/BPM)-b).^2/(2*c)) + 20.83*LED*cos((x+30)/(200*pi)) + 1770.83*LED;
    y = awgn(y, SNR, 'measured');
end

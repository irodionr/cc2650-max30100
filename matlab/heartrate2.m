function y = heartrate2(x, factor)
    peak = 0;
    beats = 0;
    %beatTime = zeros(1, 50);
    threshold = max(x(100:300)) / factor;
    
    for i = 101:200
        if peak == 0 && x(i) > threshold
            peak = 1;
            beatTime(beats+1) = i / 100;
            beats = beats + 1;
        end
        
        if peak == 1 && x(i) <= threshold
            peak = 0;
        end
    end
    
    for i = 201:1900
        threshold = max(x(i-100:i+100)) / factor;

        if peak == 0 && x(i) > threshold
            peak = 1;
            beatTime(beats+1) = i / 100;
            beats = beats + 1;
        end
        
        if peak == 1 && x(i) <= threshold
            peak = 0;
        end
    end
    
    threshold = max(x(1801:2000)) / factor;
    
    for i = 1901:2000

        if peak == 0 && x(i) > threshold
            peak = 1;
            beatTime(beats+1) = i / 100;
            beats = beats + 1;
        end
        
        if peak == 1 && x(i) <= threshold
            peak = 0;
        end
    end
    
    for i = 1:(beats-1)
        beatTime(i) = beatTime(i+1) - beatTime(i);
        beatTime(i) = 60 / beatTime(i);
    end
    
    beatTime(beats) = [];
    
    y = mean(beatTime);
end

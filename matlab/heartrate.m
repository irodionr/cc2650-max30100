function y = heartrate(x, factor)
    peak = 0;
    beats = 0;
    
    threshold = max(x(100:300)) / factor;
    
    for i = 101:200
        if peak == 0 && x(i) > threshold
            peak = 1;
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
            beats = beats + 1;
        end
        
        if peak == 1 && x(i) <= threshold
            peak = 0;
        end
    end
    
    y = beats * 6000 / 1900;
end

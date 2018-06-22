function y = threshold(x, factor)
    t = zeros(1, 2000);
    
    threshold = max(x(100:300)) / factor;
    
    for i = 100:200
        t(i) = threshold;
    end
    
    for i = 201:1900
        threshold = max(x(i-100:i+100)) / factor;
        t(i) = threshold;
    end
    
    threshold = max(x(1801:2000)) / factor;
    
    for i = 1901:2000
        t(i) = threshold;
    end
    
    y = t;
end

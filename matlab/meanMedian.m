function y = meanMedian(x, window)
    values = zeros(1, window);
    sum = 0;
    index = 1;
    count = 0;
    
    for i = 1:1:2000
        sum = sum - values(index);
        values(index) = x(i);
        sum = sum + values(index);
        
        index = index + 1;
        index = 1 + rem(index, window);
        
        if count < window
            count = count + 1;
        end
        
        avg = sum / count;
        
        x(i) = avg - x(i);
    end
    
    y = x;
end

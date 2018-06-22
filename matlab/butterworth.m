function y = butterworth(x)
    v = zeros(1, 2);
    
    for i = 1:2000
        v(1) = v(2);
        v(2) = 1.121602444751934047e-1 * x(i) + 0.77567951104961319064 * v(1);
        
        x(i) = v(1) + v(2);
    end
    
    y = x;
end

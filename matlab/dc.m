function y = dc(x, alpha)
    w = zeros(1, 10);
    
    for i = 2:1:2000
        w(i) = x(i) + alpha*w(i-1);
        x(i) = w(i) - w(i-1);
    end
    
    y = x;
end

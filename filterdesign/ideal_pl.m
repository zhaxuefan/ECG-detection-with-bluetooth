
function hd=ideal_pl(wc,M)

alpha=(M-1)/2;
n=[0:(M-1)];
m=n-alpha+eps;            %eps为一个很小的数，避免除以0
hd=sin(wc*m)./(pi*m);     %用sinc函数产生冲击响应
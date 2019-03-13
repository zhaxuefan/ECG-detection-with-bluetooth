
function z=myfilter(b,a,x)

na=numel(a);
nb=numel(b);
nx=numel(x);
nfilt=max(na,nb);

if (a(1)-1.0)>eps||(a(1)-1.0)<-eps
    for i=1:nb
        b(i)=b(i)/a(1);
    end
    for i=1:na
        a(i)=a(i)/a(1);
    end
end

%yf=zeros(1,nx+nfilt);
%xf=zeros(1,nx+nfilt);
%xf(nfilt+1:nx+nfilt)=x(:);

%af=zeros(1,nfilt);
%bf=zeros(1,nfilt);

%af(1:na)=a(:);
%bf(1:nb)=b(:);

y=zeros(1,nx);
%z=zeros(1,nx);

for i=1:nx
    %for j=1:min(i,nfilt)
        %if j<=na&&j<nb
        %    y(i)=y(i)+b(j)*x(i-j+1)-a(j)*y(i-j+1);
            
       % elseif j>na
        %    y(i)=y(i)+b(j)*x(i-j+1);
           % b(j)
        %else
        %    y(i)=y(i)-a(j)*y(i-j+1);
        %end
        
        if i<11                                     %a_index(2)
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            
        elseif i<21                                 %a_index(3)
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            y(i)=y(i)+a(11)*y(i-11+1);
        elseif i<151                                %b_index(2)
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            y(i)=y(i)+a(11)*y(i-11+1);
            y(i)=y(i)-a(21)*y(i-21+1);
        elseif i<161                                %b_index(3)
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            y(i)=y(i)+a(11)*y(i-11+1);
            y(i)=y(i)-a(21)*y(i-21+1);
            y(i)=y(i)+b(151)*x(i-151+1);
        elseif i<171                                %b_index(4)
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            y(i)=y(i)+a(11)*y(i-11+1);
            y(i)=y(i)-a(21)*y(i-21+1);
            y(i)=y(i)+b(151)*x(i-151+1);
            y(i)=y(i)-b(161)*x(i-161+1);
        elseif i<321                               %b_index(5)
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            y(i)=y(i)+a(11)*y(i-11+1);
            y(i)=y(i)-a(21)*y(i-21+1);
            y(i)=y(i)+b(151)*x(i-151+1);
            y(i)=y(i)-b(161)*x(i-161+1);
            y(i)=y(i)+b(171)*x(i-171+1);
        else                                       %
            y(i)=y(i)-b(1)*x(i-1+1);%-a(1)*y(i-1+1);
            y(i)=y(i)+a(11)*y(i-11+1);
            y(i)=y(i)-a(21)*y(i-21+1);
            y(i)=y(i)+b(151)*x(i-151+1);
            y(i)=y(i)-b(161)*x(i-161+1);
            y(i)=y(i)+b(171)*x(i-171+1);
            y(i)=y(i)-b(321)*x(i-321+1);
        end
         z(i)=y(i)/256+2;   
        
    %end
end

%{
for i=nfilt+1:nx+nfilt
    for j=1:nfilt
        if j<na&&j<nb
            yf(i)=yf(i)+bf(j)*xf(i-j+1)-af(j)*yf(i-j+1);
        elseif j>na
            yf(i)=yf(i)+bf(j)*xf(i-j+1);
        else
            yf(i)=yf(i)-af(j)*yf(i-j+1);
        end
    end
end

y=yf(nfilt+1:nx+nfilt);
%}



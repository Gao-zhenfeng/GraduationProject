clear all; close all;
% filename = '.\cornerPoint.txt';
% [xf,yf,h,s,xw,yw,zw]=textread(filename,'%n%n%n%n%n%n%n',81);
% subplot(121);
% scatter3(xw,yw,zw,'o');
% %plot(xf,yf,'o');
% hold on;
%%
% filename = '.\cornerPoint2.txt';
% [xf2,yf2,h2,s2,xw2,yw2,zw2]=textread(filename,'%n%n%n%n%n%n%n',81);
% scatter3(xw2,yw2,zw2,'o');
% hold on;

% dis = (xw2(4)-xw2(5))^2;
% dis = dis + (yw2(4)-yw2(5))^2;
% dis = dis + (zw2(4)-zw2(5))^2;
% dis = sqrt(dis);

%%
% filename = '.\hengPlane.txt';
% [a,b,c,d] = textread(filename,'%n%n%n%n',9);
% 
% for i=1:9
%     [x,y] = meshgrid(-50:1:50);
%     z = (d(i)-a(i)*x-b(i)*y)/c(i);
%     surf(x,y,z);
%     hold on;
% end



%%

filename = '.\Result.txt';
[xf,yf,h,s,x,y,z]=textread(filename,'%n%n%n%n%n%n%n',67);
scatter3(x,y,z,'.');
hold on;

planeData=[x,y,z]; 
% 协方差矩阵的SVD变换中，最小奇异值对应的奇异向量就是平面的方向
xyz0=mean(planeData,1);
centeredPlane=bsxfun(@minus,planeData,xyz0);
[U,S,V]=svd(centeredPlane);
 
a=V(1,3);
b=V(2,3);
c=V(3,3);
d=-dot([a b c],xyz0);
 
% 图形绘制
xfit = min(x):0.1:max(x);
yfit = min(y):0.1:max(y);
[XFIT,YFIT]= meshgrid (xfit,yfit);
ZFIT = -(d + a * XFIT + b * YFIT)/c;
mesh(XFIT,YFIT,ZFIT);

% for i=1:81
%     syms t;
%     n = 9*h(i)+s(i)+1;
%     a = ((xf(i)-u0)/(yf(i)-v0))*fy/fx;
%     equa = a-((x0(n)+d1(n)*t)/(y0(n)+d2(n)*t));
%     t = solve(equa);
%     `
%     xc = x0(n)+d1(n)*t;
%     yc = y0(n)+d2(n)*t;
%     zc = z0(n)+d3(n)*t;
%     
%     plot3(xc,yc,zc,'o');
%     hold on;
%     
% end
%%
% filename = '.\PointsCalib.txt';
% [h1,s1,x0,y0,z0,d1,d2,d3]=textread(filename,'%n%n%n%n%n%n%n%n',81);
% 
% %画图
% for i=1:81
%     t=-100:1:100;
%     xx=x0(i)+d1(i)*t;
%     yy=y0(i)+d2(i)*t;
%     zz=z0(i)+d3(i)*t;
%     plot3(xx,yy,zz);
%     hold on;
% end
% 

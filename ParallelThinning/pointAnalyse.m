clear ;
close all;

filename = 'keypoint.txt';
[xf,yf]=textread(filename,'%n%n');
figure;
plot(xf, yf, 'ro','LineWidth',3);
title('光条交点');
xlabel('u / pixel');
ylabel('v / pixel');

XYF = sortrows([xf, yf]);
xf = XYF(:, 1);
yf = XYF(:, 2);
figure;
xyc = [xf, yf, zeros(length(xf), 1)];

label = 1;
for i = 1 : length(xf)
    % 先判断该点是否被标记，如果未被标记，开始搜索
    if(xyc(i, 3) == 0)
        xyc(i, 3) = label; % 标记该点，表明已被分组
        classX = xyc(i, 1);
        classY = xyc(i, 2);
        for j = 1 : length(xf)
            if(j ~= i && xyc(j, 3) == 0)
                delta = xyc(i, 1) - xyc(j, 1);
                k = (xyc(j, 2) - xyc(i, 2)) / (xyc(j, 1) - xyc(i, 1));
                if(abs(delta) < 15)
                    xyc(j, 3) = label;
                    classX = [classX; xyc(j, 1)];
                    classY = [classY; xyc(j, 2)];
                end
            end
        end
        label = label + 1;
        p = plot(classX, classY, 'o', 'LineWidth',3);
        hold on
    end
    
end
% figure;
% scatter3(x,y,z,'.');
% hold on;
% 
% planeData=[x,y,z]; 
% % 协方差矩阵的SVD变换中，最小奇异值对应的奇异向量就是平面的方向
% xyz0=mean(planeData,1);
% centeredPlane=bsxfun(@minus,planeData,xyz0);
% [U,S,V]=svd(centeredPlane);
%  
% a=V(1,3);
% b=V(2,3);
% c=V(3,3);
% d=-dot([a b c],xyz0);
%  
% % 图形绘制
% xfit = min(x):0.1:max(x);
% yfit = min(y):0.1:max(y);
% [XFIT,YFIT]= meshgrid (xfit,yfit);
% ZFIT = -(d + a * XFIT + b * YFIT)/c;
% mesh(XFIT,YFIT,ZFIT);


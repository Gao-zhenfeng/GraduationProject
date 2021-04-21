clear;
close all;
filename = 'E:\课程资料\毕设\Program\Graduation_Project\ParallelThinning\k&&b.txt';
[k, b,x1, y1, x2, y2] = textread(filename, '%f%f%f%f%f%f');
K = normalize(k,'zscore', 'std') * 100;
B = normalize(b, 'zscore', 'std')* 100;
X = [K, B];
epsilon = 5;
minpts  = 2;
idx = dbscan(X, epsilon, minpts,'Distance','squaredeuclidean');

figure;
% scatter(X(:,1),X(:,2))
gscatter(X(:,1),X(:,2),idx);
title('DBSCAN Using Euclidean Distance Metric--第一次')
xlabel("k");
ylabel("b");

M = [x1, y1, x2, y2,idx];

sortM = [];
for i = 1: max(idx)
    ir = find(M(:, 5) ==i);
    sortM = [sortM; [M(ir, 1:2), M(ir, 5)]; M(ir, 3:5)];
end
B = [];
figure;
for i = 1: max(idx)
    ir = find(sortM(:, 3) ==i);
    plotX = sortM(ir, 1);
    plotY = sortM(ir, 2);
    P = polyfit(plotX, plotY,1);
    % 拼接B
    plotXY = sortrows([plotX, plotY]);
    B = [B; [plotXY, ones(length(plotX), 1) * P]];
    plot(plotXY(:, 1), plotXY(:, 2));
    title("第一次筛选");
    hold on;
end


B(:, 3) = normalize(B(:, 3) ,'zscore', 'std') * 100;
B(:, 4) = normalize(B(:, 4) ,'zscore', 'std') * 100;

epsilon2 = 3;
minpts2 = 2; % 至少为2
idx2 = dbscan(B(:, 3:4), epsilon2, minpts2,'Distance','squaredeuclidean');

figure;
% scatter(X(:,1),X(:,2))
gscatter(B(:, 3), B(:, 4),idx2);
title('DBSCAN Using Euclidean Distance Metric--第二次')
xlabel("k");
ylabel("b");

M2 = [B(:, 1), B(:, 2), B(:, 3), B(:, 4), idx2];


sortM2 = [];
figure;
for i = 1: max(idx2)
    ir2 = find(M2(:, 5) ==i);
    sortM2 = [sortM2; M2(ir2, :)];
    plotX2 = M2(ir2, 1);
    plotY2 = M2(ir2, 2);
    plotXY2 = sortrows([plotX2, plotY2]);
    plot(plotXY2(:, 1), plotXY2(:, 2));
    hold on;
    title("二次筛选");
end









%%
% 验证某张图片
clc;
close all
clear 
figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\0.5mm\light\';
filename = [root , 'l', int2str(213),'.txt'];
[u, v, xindex, yindex, x0, y0, z0] = textread(filename, '%f%f%f%f%f%f%f');
plot3(x0, y0, z0, 'o');
xlabel('X');
ylabel('Y');
zlabel('Z');
title('所有平面');
grid on
grid minor 
hold on


%%
clc;
close all
clear 
figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\20210528c\';
for i = 0 : 34
    filename = [root , 'Image', int2str(i),'.txt'];
    [u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
    plot3(x1, y1, z1, 'o');
    xlabel('X');
    ylabel('Y');
    zlabel('Z');
    title('所有平面');
    grid on
    grid minor 
    hold on
end

figure;
% for i = 0 : 50 % 
%     filename = [root , 'Image', int2str(i),'.txt'];
%     [u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
%     plot(u(7900 : 7950, 1), v(7900 : 7950, 1) ,'o','MarkerSize', 4);
%     xlabel('X');
%     ylabel('Y');
%     title('第一条光条');
%     hold on
% end

filename = [root , 'Image', int2str(0),'.txt'];
[u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
plot(u(779 : 798, 1), v(779 : 798, 1) ,'o','MarkerSize', 4);
xlabel('X');
ylabel('Y');
hold on
filename = [root , 'Image', int2str(51),'.txt'];
[u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
plot(u(751 : 770, 1), v(751 : 770, 1) ,'o','MarkerSize', 4);
hold on
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\20210528c\Helmet\';
filename2 = 'E:\课程资料\毕设\Program\Graduation_Project\Data\20210528c\Helmet\l159.txt';
[u, v, xindex, yindex, x0, y0, z0] = textread(filename2, '%f%f%f%f%f%f%f');
plot(u(8014 : 8203, 1), v(8014 : 8203, 1) ,'o','MarkerSize',3);
title('标定像面坐标');


figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\20210528c\';
filename = [root , 'Image0.txt'];
[u1, v1, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
XX1 = u1(1 : 100, 1);
YY1 = v1(1 : 100, 1);
plot3(x1, y1, z1, 'o');
xlabel('X');
ylabel('Y');
zlabel('Z');
hold on
filename = [root , 'Image1.txt'];
[u, v, xindex, yindex, x2, y2, z2] = textread(filename, '%f%f%f%f%f%f%f');
plot3(x2, y2, z2, 'o');
title('实际两张图点云');


figure;
filename = [root , 'Image', int2str(3),'.txt'];
[u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
plot3(x1, y1, z1, 'o');
hold on
filename = [root , 'light/l8.txt'];
[u, v, xindex, yindex, x3, y3, z3] = textread(filename, '%f%f%f%f%f%f%f');
plot3(x3, y3, z3, 'o');

title('实际与测量');

%%
% 读取5MM 文件夹内数据
clc;
close all
clear 
figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\0.5mm\5MM\5mm\';
filename = [root , 'l1 - Cloud.txt'];
[x0, y0, z0] = textread(filename, '%f%f%f');
plot3(x0, y0, z0, 'o');
xlabel('X');
ylabel('Y');
zlabel('Z');
title('所有平面');
grid on
grid minor 
hold on

filename = [root,'l2 - Cloud.txt'];
[x1, y1, z1] = textread(filename, '%f%f%f');
plot3(x1, y1, z1, 'o');
hold on

filename = [root ,'l3 - Cloud.txt'];
[x2, y2, z2] = textread(filename, '%f%f%f');
plot3(x2, y2, z2, 'o');
hold on

filename = [root ,'l4 - Cloud.txt'];
[x3, y3, z3] = textread(filename, '%f%f%f');
plot3(x3, y3, z3, 'o');
hold on

filename = [root ,'l5 - Cloud.txt'];
[x4, y4, z4] = textread(filename, '%f%f%f');
plot3(x4, y4, z4, 'o');
hold on

%%
% 头盔
clc;
close all
clear;
figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\0.5mm\Helmet\';
name = 'l166';
filename = [root , name,'.txt'];
[u, v, xindex, yindex, x0, y0, z0] = textread(filename, '%f%f%f%f%f%f%f');
plot3(x0, y0, z0, 'o','MarkerSize',1);
xlabel('X');
ylabel('Y');
zlabel('Z');

figure;
ptCloud = pointCloud([-single(x0), -single(y0), -single(z0)]);
pcwrite(ptCloud, [root, name, '.pcd'], 'Encoding', 'ascii'); %将程序中的xyz数据写入pcd文件中
pc = pcread([root, name, '.pcd']);
pcshow(pc); %显示点云
hold on
pc2 = pcread([root, 'Helmet-y.pcd']);
% pcshow(pc2); %显示点云
[tform,movingReg] = pcregistericp(pc,pc2);
pointscolor=uint8(zeros(movingReg.Count,3));
pointscolor(:,1)=255;
pointscolor(:,2)=0;
pointscolor(:,3)=0;
movingReg.Color=pointscolor;
% pcshow(movingReg);
%%
% 参考点云转化PCD文件
figure;
root1 = 'E:\课程资料\毕设\Program\PointCloudData\';
filename = [root1 , 'Helmet-y.txt'];
[x, y, z, R, G, B] = textread(filename, '%f%f%f%f%f%f');
plot3(x, y, z, 'o');
xlabel('X');
ylabel('Y');
zlabel('Z');
x = single(x);
y = single(y);
z = single(z);
ptCloud = pointCloud([x, y, z]);
pcwrite(ptCloud, [root,'Helmet-y.pcd'], 'Encoding', 'ascii'); %将程序中的xyz数据写入pcd文件中
pc = pcread('Helmet-y.pcd');
pcshow(pc); %显示点云

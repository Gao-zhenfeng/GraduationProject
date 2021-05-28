clc;
close all
clear 
figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\20210526\';
for i = 0 : 7
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
for i = 0 : 7 % 
    filename = [root , 'Image', int2str(i),'.txt'];
    [u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
    % 第0条光条
    plot(u(1 : 200, 1), v(1 : 200, 1) ,'o','MarkerSize',6);
    xlabel('X');
    ylabel('Y');
    title('第一条光条');
    grid on
    grid minor 
    hold on
end

figure;
for i = 0 : 6
    filename = [root , 'Image', int2str(i),'.txt'];
    [u, v, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
    % 第0条光条
    plot(u(21 : 40, 1), v(21 : 40, 1) ,'o', 'MarkerSize',6);
    grid on
    grid minor 
    xlabel('X');
    ylabel('Y');
    title('第二条光条');
    hold on
end


figure;
filename = [root , 'Image0.txt'];
[u1, v1, xindex, yindex, x1, y1, z1] = textread(filename, '%f%f%f%f%f%f%f');
XX1 = u1(1 : 20, 1);
YY1 = v1(1 : 20, 1);
plot3(x1, y1, z1, 'o');
xlabel('X');
ylabel('Y');
zlabel('Z');
hold on

filename = [root , 'Image1.txt'];
[u, v, xindex, yindex, x2, y2, z2] = textread(filename, '%f%f%f%f%f%f%f');
XX2 = u1(1 : 20, 1);
YY2 = v1(1 : 20, 1);
plot3(x2, y2, z2, 'o');
title('两张图');
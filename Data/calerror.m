clc;
close all;
clear;
filename1 = 'E:\课程资料\毕设\Program\Graduation_Project\Data\RTCorner1.txt';
[u, v, xindex, yindex, x1, y1, z1] = textread(filename1, '%f%f%f%f%f%f%f');
plot3(x1, y1, z1, '+');
hold on

filename2 = 'E:\课程资料\毕设\Program\Graduation_Project\Data\RTCorner@1.txt';
[x2, y2, z2] = textread(filename2, '%f%f%f');
plot3(x2, y2, z2, 'o');
xlabel('X');
ylabel('Y');
zlabel('Z');
grid on
hold on

deltax = x1 - x2;
deltay = y1 - y2;
deltaz = z1 - z2;
figure;
errs = sqrt(deltax .* deltax + deltay .* deltay + deltaz .* deltaz);
plot3(xindex, yindex, errs, 'o');
grid on
xlabel('X');
ylabel('Y');
zlabel('Error');
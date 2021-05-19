clc;
close all;
clear;
I1 = imread('I1.bmp');
I2 = imread('I2.bmp');
I3 = imread('E:\课程资料\毕设\开题报告&论文\毕业论文\DIP3E_CH09_Art_PowerPoint\threshold.bmp');

figure;
L = logical(I3);
imshow(L);

figure;
I = I1 - I2;
imshow(I);

figure;
% imwrite(I, 'region1.bmp');
BW = imbinarize(I, 'global');
imshow(BW);

figure;
src = imread('E:\课程资料\毕设\Program\Graduation_Project\Picture\I10.bmp');
ginf = bwmorph(src, 'thin', inf);
imshow(ginf);

figure;
imhist(I);
figure;
x1 = linspace(-5, 5, 100);
x2 = linspace(-5, 5, 100);
[X,Y]=meshgrid(x1,x2);
Z= 1.5 * X.* X + 0.5* X.* Y + Y.*Y;

mesh(x1, x2, Z);
xlabel('x');
ylabel('y');
zlabel('z');
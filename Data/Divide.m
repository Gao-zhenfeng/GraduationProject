clc;
close all;
clear;
I1 = imread('I1.bmp');
I2 = imread('I2.bmp');
I3 = imread('C:\Users\Amadeus\Desktop\DIP3E_CH09_Art_PowerPoint\threshold.bmp');
figure;
imshow(I3);
L = logical(I3);
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
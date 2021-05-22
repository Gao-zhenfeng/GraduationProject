clc;
close all;
clear;

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image0.txt';
[x, y, z] = textread(filename, '%f%f%f');
plot3(x, y, z, 'o');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image1.txt';
[x, y, z] = textread(filename, '%f%f%f');
plot3(x, y, z, '+');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image2.txt';
[x, y, z] = textread(filename, '%f%f%f');
plot3(x, y, z, '*');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image3.txt';
[x, y, z] = textread(filename, '%f%f%f');
plot3(x, y, z, 's');
hold on

clc;
clear;
close all;

% 灰度图， uint8 1024 * 1280
src = imread('E:\课程资料\毕设\Program\Graduation_Project\Picture\I8.bmp');
figure;
subplot(2,2,1);
imshow(src);
subplot(2,2,2);
imhist(src); % 像素直方图
J1 = histeq(src); % 直方图均衡增强对比度
subplot(2,2,3);
imshow(J1);
subplot(2,2,4);
imhist(J1);


% 对J1(直方图均衡化）进行自适应二值化， 得到bw2
bw2 = imbinarize(J1, 'adaptive');
bw2 = medfilt2(bw2, [3, 3]);

bw3 = im2bw(J1, 150/255);
bw3 = medfilt2(bw3, [3, 3]);

figure;
set(gcf, 'position', get(0,'ScreenSize'));
subplot(2,4,1);
imshow(J1);
title('直方图均衡');
subplot(2,4,2);
imshow(bw2);
title('自适应二值化');

subplot(2,4,8);
imshow(bw3);
title('普通二值化');

J2 = medfilt2(bw2, [5, 5]);
subplot(2,4,3);
filterJ2 = J2;
imshow(filterJ2);
title('二值图像直接滤波');

subplot(2,4,4);
imshow(src);
title('原图');

% J3 = medfilt2(bw2, [3, 3]);
BG = imopen(bw2, strel('disk', 15));
BG = medfilt2(BG, [10,10]);
subplot(2,4,5);
imshow(BG);
title('背景');

J3 = imsubtract(bw2, BG);
subplot(2,4,6);
imshow(J3); 
title('背景差分');
L = logical(J3);

J3 = medfilt2(J3, [7, 7]);
J3 = medfilt2(J3, [3, 3]);
subplot(2,4,7);
imshow(J3);
title('滤波');


[labeled, numObjects] = bwlabel(J3, 8);
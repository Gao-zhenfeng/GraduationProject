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
J1 = imadjust(src); % 直方图均衡增强对比度
subplot(2,2,3);
imshow(J1);
subplot(2,2,4);
imhist(J1);

edgeSrc = edge(src, 'canny');

figure;
% set(gcf, 'position', get(0,'ScreenSize'));
subplot(2,4,1);

imshow(src);
title('原图');

% J3 = medfilt2(bw2, [3, 3]);
BG = imopen(src, strel('disk', 9));
% BG = medfilt2(BG, [10,10]);
subplot(2,4,2);
imshow(BG);
title('背景');

[r, c] = size(BG);
for i = 1 : r
   for j = 1 : c  
        if(BG(i, j) < 30)
            BG(i, j) = 0;
        end
   end
end
subplot(2,4,8);
imshow(BG);
title('背景滤波');

% J2 = imsubtract(src, BG);
J2 = imtophat(src, strel('disk', 15));
subplot(2,4,3);
imshow(J2); 
title('背景差分');
L = logical(J2);

% 对src进行自适应二值化， 得到bw2
bw2 = imbinarize(J2, 'adaptive');
% bw2 = medfilt2(bw2, [3, 3]);
subplot(2,4,4);
imshow(bw2); 
title('自适应二值化');


J3 = medfilt2(bw2, [5, 5]);
% J3 = medfilt2(J3, [3, 3]);
subplot(2,4,5);
imshow(J3);
title('滤波');


[imlabel, numObjects] = bwlabel(bw2, 4);
stats = regionprops(imlabel, 'Area');
area = cat(1, stats.Area);
index = find(area == max(area));
img = ismember(imlabel, index);
figure;
subplot(1,2,1);
imshow(src);
subplot(1,2,2);
imshow(img);

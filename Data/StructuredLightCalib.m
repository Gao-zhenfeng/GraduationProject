%%
clc;
close all;
clear;
filename = 'E:\课程资料\毕设\Program\Graduation_Project\Data\calib.txt';
[x0, y0, z0, d1, d2, d3] = textread(filename, '%f%f%f%f%f%f');
t = 10;
x = x0 + t * d1;
y = y0 + t * d2;
z = z0 + t * d3;
plot3(x, y, z, 'o');
ax = gca;
ax.XAxisLocation = 'origin';
ax.YAxisLocation = 'origin';
 
hold on
t = 5;
x = x0 + t * d1;
y = y0 + t * d2;
z = z0 + t * d3;
plot3(x, y, z, 'o');
hold on

t = 15;
x = x0 + t * d1;
y = y0 + t * d2;
z = z0 + t * d3;
plot3(x, y, z, 'o');


%%
clc;
close all;
clear;
filename = 'E:\课程资料\毕设\Program\Graduation_Project\Data\allcornerPoint.txt';
[u, v, labelRow, labelCol, x, y, z] = textread(filename, '%f%f%f%f%f%f%f');
maxRow = max(labelRow);
maxCol = max(labelCol);
data = [];
index = 1;

for i = 0 : maxRow
    for j = 0 : maxCol
        u0 = 0;v0 = 0;x0 = 0;y0 = 0;z0 = 0;
        index = 0;
       for k = 1 : length(u)
          if(labelRow(k) == i && labelCol(k) == j)
              u0 = u0 + u(k);
              v0 = v0 + v(k);
              x0 = x0 + x(k);
              y0 = y0 + y(k);
              z0 = z0 + z(k);
              index = index + 1;
          end
       end
       data(15 * (i) + j + 1, 1) = u0 / index;
       data(15 * (i) + j + 1, 2) = v0 / index;
       data(15 * (i) + j + 1, 3) = i;
       data(15 * (i) + j + 1, 4) = j;
       data(15 * (i) + j + 1, 5) = x0 / index;
       data(15 * (i) + j + 1, 6) = y0 / index;
       data(15 * (i) + j + 1, 7) = z0 / index;
     end
        
end
clear u; clear v; clear x; clear y; clear z;clear labelCol; clear labelRow;
% fileID = fopen('cornerPoint.txt', 'w');
% fprintf(fileID, '%-13.6f%-13.6f%-13.6f%-13.6f%-13.6f%-13.6f%-13.6f\n', data');


plot3(data(:, 5), data(:, 6), data(:, 7), 'o');
hold on

x = data(1 : 15, 5);
y = data(1 : 15, 6);
z = data(1 : 15, 7);


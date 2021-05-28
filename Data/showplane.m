 %%
figure;
root = 'E:\课程资料\毕设\Program\Graduation_Project\Data\20210522a\';
filename = [root , 'Image6.txt'];
[x1, y1, z1] = textread(filename, '%f%f%f');
plot3(x1, y1, z1, 'o');
xlabel('X');
ylabel('Y');
hold on

filename = [root , 'Image0.txt'];
[x2, y2, z2] = textread(filename, '%f%f%f');
plot3(x2, y2, z2, '+');
hold on

filename = [root , 'Image1.txt'];
[x3, y3, z3] = textread(filename, '%f%f%f');
plot3(x3, y3, z3, '*');
hold on

filename = [root , 'Image2.txt'];
[x4, y4, z4] = textread(filename, '%f%f%f');
plot3(x4, y4, z4, 's');
hold on

filename = [root , 'Image3.txt'];
[x5, y5, z5] = textread(filename, '%f%f%f');
plot3(x5, y5, z5, 's');
hold on

filename = [root , 'Image4.txt'];
[x6, y6, z6] = textread(filename, '%f%f%f');
plot3(x6, y6, z6, 'o');
hold on

filename = [root , 'Image5.txt'];
[x7, y7, z7] = textread(filename, '%f%f%f');
plot3(x7, y7, z7, '+');
hold on
%%
clc;
clear;
figure;
filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image0.txt';
[x1, y1, z1] = textread(filename, '%f%f%f');
plot3(x1, y1, z1, 'o');
xlabel('X');
ylabel('Y');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image1.txt';
[x2, y2, z2] = textread(filename, '%f%f%f');
plot3(x2, y2, z2, '+');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image2.txt';
[x3, y3, z3] = textread(filename, '%f%f%f');
plot3(x3, y3, z3, '*');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image3.txt';
[x4, y4, z4] = textread(filename, '%f%f%f');
plot3(x4, y4, z4, 's');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image4.txt';
[x5, y5, z5] = textread(filename, '%f%f%f');
plot3(x5, y5, z5, 's');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image5.txt';
[x6, y6, z6] = textread(filename, '%f%f%f');
plot3(x6, y6, z6, 'o');
hold on

filename = 'E:\课程资料\毕设\Program\Graduation_Project\StructuredLineCalibration\Image6.txt';
[x7, y7, z7] = textread(filename, '%f%f%f');
plot3(x7, y7, z7, '+');
hold on
RSME = [];
COEFFS = [];
for i = 1 : 20
    X = [x1(1 + (i -1) * 20 : i * 20, 1); x2(1 + (i -1) * 20 : i * 20, 1); x4(1 + (i -1) * 20 : i * 20, 1); x5(1 + (i -1) * 20 : i * 20, 1); x6(1 + (i -1) * 20 : i * 20, 1); x7(1 + (i -1) * 20 : i * 20, 1)];
    Y = [y1(1 + (i -1) * 20 : i * 20, 1); y2(1 + (i -1) * 20 : i * 20, 1); y4(1 + (i -1) * 20 : i * 20, 1); y5(1 + (i -1) * 20 : i * 20, 1); y6(1 + (i -1) * 20 : i * 20, 1); y7(1 + (i -1) * 20 : i * 20, 1)];
    Z = [z1(1 + (i -1) * 20 : i * 20, 1); z2(1 + (i -1) * 20 : i * 20, 1); z4(1 + (i -1) * 20 : i * 20, 1); z5(1 + (i -1) * 20 : i * 20, 1); z6(1 + (i -1) * 20 : i * 20, 1); z7(1 + (i -1) * 20 : i * 20, 1)];

    [xData, yData, zData] = prepareSurfaceData( X, Y, Z );
    % Set up fittype and options.`
    ft = fittype( 'poly11' );
    opts = fitoptions( 'Method', 'LinearLeastSquares' );
    opts.Robust = 'Bisquare';
    MAX = max([xData, yData]);
    MIN = min([xData, yData]);
    % Fit model to data.
    [fitresult, gof] = fit( [xData, yData], zData, ft, opts );
    RSME = [RSME,gof.rmse];
 
    p = coeffvalues(fitresult);
    COEFFS = [COEFFS; p];
    [XX,YY] = meshgrid(xData, yData);
    ZZ =  p(1) + p(2).* XX + p(3).*YY;  
    mesh(XX, YY, ZZ);
    xlabel( 'X', 'Interpreter', 'none' );
    ylabel( 'Y', 'Interpreter', 'none' );
    zlabel( 'Z', 'Interpreter', 'none' );
    view( -76.9, 2.8 );
    hold on
end

    



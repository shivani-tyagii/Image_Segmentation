clc
clear

I=imread('p1.jpg');
imshow(I);
I=double(I);
class_number=2;
potential=0.5;
maxIter=90;
seg=ICM(I,class_number,potential,maxIter);
figure;
imshow(seg,[]);
function [ centroids] = init_centroids(X,k)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

centroids = zeros(k,size(X,2)); 
randidx = randperm(size(X,1));
centroids = X(randidx(1:k), :);





end


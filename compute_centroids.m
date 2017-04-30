function [ centroids ] = compute_centroids( X,idx,k )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

 [m n] = size(X);
  centroids = zeros(k, n);
  
  for i=1:k
    xi = X(idx==i,:);
    ck = size(xi,1);
    centroids(i, :) = (1/ck) * sum(xi);
end



end


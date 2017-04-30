function [ indices ] = run_kmean( X, k )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

max_itr=10;
centroids=init_centroids(X,k);

for i=1:max_itr
    indices=get_closest_centroid(X,centroids);
    centroids=compute_centroids(X,indices,k);
end




end
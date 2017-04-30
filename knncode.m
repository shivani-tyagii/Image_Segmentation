function test_lbls = knncode(test_data, tr_data,tr_lbls,k)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
test_data = reshape(test_data,[],1);
tr_data = reshape(tr_data,[],1);
tr_lbls = reshape(tr_lbls,[],1);

num_test_data = size(test_data,1);

euclidean_dst = zeros(int8(num_test_data/2),1);
test_lbls = zeros(num_test_data,1);
for sample=2:2:num_test_data

   %% Step 1: Computing euclidean distance between each test_data sample and
   % training data
    j = 1;
    for i=1:2:num_test_data 
        euclidean_dst(j) = abs( test_data(sample) - tr_data(i) );
        j=j+1;
    end
    
    %% Step 2: compute k nearest neighbors, and store there corresponding
    % labels in k_nrst_nghbors_lbls
    [~, position] = sort(euclidean_dst);
    k_nrst_nghbors = position(1:k);
    for i=1:k
        k_nrst_nghbors(i)=2*k_nrst_nghbors(i)-1;
    end
    k_nrst_nghbors_lbls = tr_lbls(k_nrst_nghbors);
    
    %% Step 3 : Voting - choosing the most frequent label, if the labels are
    % unique, vote for the nearest neighbor label in the neighborhood
    [M,F] = mode(k_nrst_nghbors_lbls);
    if F~=1
        test_lbls(sample)=M;
    else
        test_lbls(sample)=k_nrst_nghbors_lbls(1);
    end
    
end
end


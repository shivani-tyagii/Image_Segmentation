k=3;
im_test_data = [10 20 30; 20 10 30; 20 10 30];
imshow(im_test_data);figure;
im_tr_data = [10 0 30;0 10 0; 20 0 30];
im_t_lbls = [1 2 3; 2 1 3; 2 1 3];
im_tr_lbls = [1 0 3;0 1 0; 2 0 3];
test_lbls = knncode(im_test_data, im_tr_data,im_tr_lbls,k);
imshow(test_lbls);
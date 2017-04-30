img = imread('images1.jpeg');
%putting image to another variable
lab_he=img;
size(img)
ab = double(lab_he(:,:,2:3));
size(ab)
nrows = size(ab,1);
ncols = size(ab,2);
ab = reshape(ab,nrows*ncols,2);
size(ab)
nColors = 6;
[cluster_idx] = run_kmean(ab,6);
   
pixel_labels = reshape(cluster_idx,nrows,ncols);
imshow(pixel_labels,[]), title('image labeled by cluster index');figure;
segmented_images = cell(1,10);
rgb_label = repmat(pixel_labels,[1 1 3]);

for k = 1:nColors
    color = img;
    color(rgb_label ~= k) = 0;
    segmented_images{k} = color;
     imshow(segmented_images{k});figure;
    
end

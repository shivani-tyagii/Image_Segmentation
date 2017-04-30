
x=imread('i2.png');
figure; imshow(x);
[r,c,s]=size(x);

%initialize storage for each sample region
classes={'red','orange','blue','yellow','green','light_blue','white','background'};
nclasses=length(classes);
sample_regions=false([r c nclasses]);

%select each sample region
f=figure;
for count=1:nclasses
    set(f,'name',['select sample region for' classes{count}]);
    sample_regions(:,:,count)=roipoly(x);
end
close(f);
%convert an RGB age into an L*a*b image

cform=makecform('srgb2lab');
lab_x=applycform(x,cform);

% calculate mean for each area

a=lab_x(:,:,2);
b=lab_x(:,:,3);
color_markers=repmat(0,[nclasses,2]);
for count = 1:nclasses
    color_markers(count,1)=mean2(a(sample_regions(:,:,count)));
    color_markers(count,2)=mean2(b(sample_regions(:,:,count)));
end

color_labels=0:nclasses-1;
a=double(a);
b=double(b);
distance=repmat(0,[size(a), nclasses]);

%classification

for count= 1:nclasses
    distance(:,:,count)= ( (a-color_markers(count,1)).^2 + (b - color_markers(count,2)).^2).^0.5;
end

[vlaue, label]=min(distance,[],3);
label= color_labels(label);

%clear value distance;

colors= [255 0 0 ; 0 255 0; 255 255 255; 255 255 0; 255 0 255; 0 255 255; 0 0 255; 0 0 0];
y=zeros(size(x));
l=double(label)+1;
 for m=1:r
     for n=1:c
         y(m,n,:)=colors(l(m,n),:);
     end
 end

figure ; imshow(y)
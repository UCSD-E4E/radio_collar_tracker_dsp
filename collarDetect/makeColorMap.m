function [img] = makeColorMap(lat, lon, col, noise, scale)
img = ones(floor((max(lon) - min(lon))/scale) + 1, floor((max(lat) - min(lat))/scale) + 1) * min(min(noise), min(col));
for i = 1:1:length(noise)
    img(floor((lon(i) - min(lon))/scale) + 1, floor((max(lat) - min(lat))/scale) - floor((lat(i) - min(lat))/scale) + 1) = col(i);
end
colormap('jet');
imagesc(img);
clear img;
end
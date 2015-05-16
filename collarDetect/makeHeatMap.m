function [] = makeHeatMap(lat, lon, col)
caxis([min(col), max(col)]);
colormap(jet);
x = zeros(length(lat), 1);
y = zeros(length(lat), 1);
for i = 1:1:length(lat)
    [xu,yu] = deg2utm(lat(i), lon(i));
    x(i) = xu;
    y(i) = yu;
end
maxCol = max(col);
minCol = min(col);
curColMap = colormap;
curmapsize = size(curColMap,1);
mapz = round(1 + (col - minCol) ./ (maxCol-minCol) .* (curmapsize-1));
scatter(x, y, 16, curColMap(mapz,:)); % plot x and y with 16 unit area, color specified by colormap
axis image; %sets axis increments and scale equal to each other
grid on;
colorbar; % creates a colorbar
end
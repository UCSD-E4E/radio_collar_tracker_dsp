function [] = makeHeatMap(lat, lon, col)
colormap(jet);
x = zeros(length(lat));
y = zeros(length(lat));
for i = 1:1:length(lat)
    [xu,yu] = deg2utm(lat(i), lon(i));
    x(i) = xu;
    y(i) = yu;
end
scatter(x, y, 16, col);
axis equal;
colorbar
end
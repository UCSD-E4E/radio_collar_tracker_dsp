function [] = makeHeatMap(lat, lon, col)
colormap(jet);
scatter(lat, lon, 16, col);
axis equal;
colorbar
end
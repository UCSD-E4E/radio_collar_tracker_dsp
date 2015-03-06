function [] = makeRunHeatMap()
[file, path] = uigetfile('*.csv');
data = csvread(strcat(path, file));
lat = data(:,1) / 10000000;
lon = data(:,2) / 10000000;
col1 = data(:,9)/1000;
col2 = data(:,10)/1000;
col3 = data(:,11)/1000;
col4 = data(:,12)/1000;
col5 = data(:,13)/1000;
col6 = data(:,14)/1000;
noise = data(:,15)/1000;
figure;
hold on;
subplot(2, 3, 1);
hold on;
makeHeatMap(lat, lon, col1);
scatter(32.716683, -115.93375, 25, 'k', 'x')
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Collar 1');
colorbar;
subplot(2, 3, 2);
hold on;
makeHeatMap(lat, lon, col2);
scatter(32.716809, -115.93378, 25, 'k', 'x')
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Collar 2');
colorbar;
subplot(2, 3, 3);
hold on;
makeHeatMap(lat, lon, col3);
scatter(32.716317, -115.93342, 25, 'k', 'x')
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Collar 3');
colorbar;
subplot(2, 3, 4);
hold on;
makeHeatMap(lat, lon, col4);
scatter(32.716969, -115.93327, 25, 'k', 'x')
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Collar 4');
colorbar;
subplot(2, 3, 5);
hold on;
makeHeatMap(lat, lon, col5);
scatter(32.717086, -115.93286, 25, 'k', 'x')
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Collar 5');
colorbar;
subplot(2, 3, 6);
hold on;
makeHeatMap(lat, lon, col6);
scatter(32.717202, -115.93291, 25, 'k', 'x')
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Collar 6');
colorbar;
figure;
makeHeatMap(lat, lon, noise);
caxis([min(min(data(:,9:15)))/1000, max(max(data(:,9:15)))/1000]);
title('Noise');
colorbar;
clear lat lon col1 col2 col3 col4 col5 col6 noise
end
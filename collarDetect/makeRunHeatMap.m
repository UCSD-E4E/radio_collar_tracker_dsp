function [] = makeRunHeatMap(varargin)
numRowPlots = 1;
numColPlots = 1;
numCols = 1;
firstCollarColumn = 6;
file = 0;
path = 0;
data = 0;
% Getting file
if nargin == 0
    [file, path] = uigetfile({'*.csv';'*.csv.bak'});
    % get data from file
    if path == 0
        return
    end
    data = csvread(strcat(path, file));
elseif nargin == 1
    path = char(varargin(1));
    data = csvread(path);
end


% Extract data
lat = data(:,1) / 10000000;
lon = data(:,2) / 10000000;
col = zeros(length(lat), numCols);
for i = 1:1:numCols
    tmp = firstCollarColumn - 1 + i;
    col(1:length(lat), i) = transpose(data(:,tmp)/1000);
end
noise = data(:,firstCollarColumn + i)/1000;

% create plot
figure;
hold on;
for i = 1:1:numCols
    % create a 2 row x 3 col subplot, select plot 1
    subplot(numRowPlots, numColPlots, i);
    hold on;
    makeHeatMap(lat, lon, col(:,i));
    caxis([min(min(data(:,firstCollarColumn:firstCollarColumn+numCols)))/1000, max(max(data(:,firstCollarColumn:firstCollarColumn+numCols)))/1000]);
    title(strcat('Collar ',num2str(i)));
    colorbar;
end

figure;
makeHeatMap(lat, lon, noise);
caxis([min(min(data(:,firstCollarColumn:firstCollarColumn+numCols)))/1000, max(max(data(:,firstCollarColumn:firstCollarColumn+numCols)))/1000]);
title('Noise');
colorbar;
end
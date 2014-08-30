close all
clear
clc
% Process Parameters
FP_PREC       	= 'double'; 
GT_PLOT         = 0; 

% File Parameters
CSV_PREFIX   	= 'RUN_'; 
META_PREFIX   	= 'META_'; 
KML_PREFIX   	= 'KML_'; 
JOB_FILE_NAME 	= 'JOB'; 
CSV_NUM_PREC    = 20;
SNR_THRES       = 0.01; 

% Distance Analysis parameters
USE_GT          = 0;  
HIST_W          = 7; 
HOR_D           = 0;  
MAX_SCATTER_W   = 128; 
SCATTER_W       = 64; 

% Run Data
gty_d           = [33.06608; 33.06495; 33.06645];
gtx_d           = [-117.10118; -117.10222; -117.10106];
gty             = gty_d*10^7;
gtx             = gtx_d*10^7;
ALTITUDE_FT     = 150;
MIN_ALT_FACT    = 0.63;  
POLY_ORDER      = 1; 

alt_m = ALTITUDE_FT*0.3048; 

alt_thr = MIN_ALT_FACT*alt_m; 

% Output Image Parameters
IMG_PREFIX     	= 'IMG_RUN_';

clc
fprintf('Process Started...\n'); 

% Job File Loading
job_file     	= fopen(JOB_FILE_NAME); 
fgetl(job_file);
aux          	= textscan(job_file, '%s%d'); 
cfg          	= aux{2};
fclose(job_file);

% Job parameters loading
curr_run        = cast(cfg(1),FP_PREC);
lin_scale   	= cast(cfg(4),FP_PREC);
map_d          	= cast(cfg(5),FP_PREC);
alpha_c_thres  	= cast(cfg(6),FP_PREC);
num_col        	= cast(cfg(7),FP_PREC); 

% Data Loading
aux          	= num2str(curr_run,'%06d'); 
csv_filename  	= strcat(CSV_PREFIX, aux, '.csv');
aux           	= csvread(csv_filename);
ttl_n_fr      	= size(aux, 1);
min_alt         = min(aux(:, 3))/1000;

% Eliminating low altitudes
count = 1; 
for i=1:ttl_n_fr
    if aux(i, 3)/1000 > alt_thr+min_alt
        aux1(count,:) = aux(i,:);
        count = count + 1; 
    end
end

ttl_n_fr      	= size(aux1, 1);
gps_lat       	= aux1(:, 1);
gps_lon        	= aux1(:, 2);
gps_alt        	= aux1(:, 3); 
pulse_snr     	= aux1(:, 4:end)/1000; 
clear aux;

gps_lat_d       = gps_lat/10^7;
gps_lon_d       = gps_lon/10^7;

% KML writting
aux          	= num2str(curr_run,'%06d'); 
kml_filename  	= strcat(KML_PREFIX, aux, '.kml');
kmlwrite(kml_filename,gps_lat_d, gps_lon_d);

% MinMaxDelta Position
max_lat         = max(gps_lat); 
min_lat         = min(gps_lat);
del_lat         = max_lat - min_lat;

max_lon         = max(gps_lon); 
min_lon         = min(gps_lon);
del_lon         = max_lon - min_lon;

max_alt         = max(gps_alt); 
min_alt         = min(gps_alt);
del_alt         = max_alt - min_alt;

% Data allocation
dist            = zeros(ttl_n_fr, num_col);
error           = zeros(ttl_n_fr, num_col);

% Getting values in meters
[min_lon_m min_lat_m gps_lon_m gps_lat_m ] = ...
    degressToMeters( min_lon, min_lat, gps_lon-min_lon, gps_lat-min_lat);
% Getting values in meters
[min_lon_m min_lat_m gtx_m gty_m ] = ...
    degressToMeters( min_lon, min_lat, gtx-min_lon, gty-min_lat);

% Relative degrees positions
x_p             = cast( gps_lon-min_lon, FP_PREC)'; 
y_p             = cast( gps_lat-min_lat, FP_PREC)'; 

% Voronoi Diagram Calculation
area = zeros(num_col, 1); 
[v c] = voronoin([gps_lon gps_lat]);
for i=1:ttl_n_fr
    cur_cel = c{i}; 
    area(i) = polyarea(v(cur_cel,1), v(cur_cel,2)); 
    if isnan(area(i))
        area(i) = 0; 
    end
end

% Grid positions to evaluate the spline
del_lon_fp      = cast(del_lon, FP_PREC); 
del_lat_fp      = cast(del_lat, FP_PREC); 
step_x          = del_lon_fp/map_d; 
step_y          = del_lat_fp/map_d; 
grid_pos_x      = cast(1:step_x:del_lon_fp, FP_PREC);
grid_pos_y      = cast(1:step_y:del_lat_fp, FP_PREC);
grid_pos        = zeros(2, map_d.^2);
for i = 1:map_d
    ini_el      = (i-1)*map_d+1; 
    fin_el      = (i-1)*map_d+map_d; 
    grid_pos(:, ini_el:fin_el)=[grid_pos_x(i)*ones(1, map_d); grid_pos_y];
end

% dB to linear SNR conversion
if lin_scale
    pulse_snr   = 10.^(pulse_snr/10);
end

% MinMaxDelta SNR
max_snr         = max(pulse_snr); 
min_snr         = min(pulse_snr); 
del_snr         = max_snr - min_snr;

% Memmory Allocation
centr_x             = zeros(num_col, 1); 
centr_y             = zeros(num_col, 1);
centr_x_d           = zeros(num_col, 1); 
centr_y_d           = zeros(num_col, 1);
max_x_coor          = zeros(num_col, 1); 
max_y_coor          = zeros(num_col, 1); 
dist_s              = zeros(map_d*map_d, num_col);
snr_s               = zeros(map_d*map_d, num_col); 
interp_eval         = zeros(ttl_n_fr,num_col);
snr_1_0_scale       = zeros(ttl_n_fr,num_col);

for l=1:num_col
    
    % 1-0 scale SNR
    snr_1_0_scale(:,l)   = (pulse_snr(:,l) - min_snr(:,l))/del_snr(:,l);
    pulse_snr_log   = log10(pulse_snr);

    % Centroid calc
    aux1 = 0;
    aux2 = 0;
    aux3 = 0;
    for m=1:ttl_n_fr
        if snr_1_0_scale(m,l) > SNR_THRES
            aux1 = aux1 + ...
                snr_1_0_scale(m,l).^2;
            aux2 = aux2 + ...
                snr_1_0_scale(m,l).^2.*cast(gps_lon(m)-min_lon, FP_PREC);
            aux3 = aux3 + ...
                snr_1_0_scale(m,l).^2.*cast(gps_lat(m)-min_lat, FP_PREC);
        end
    end
    centr_x(l)  = cast(aux2/aux1 + min_lon,'int64');  
    centr_y(l)  = cast(aux3/aux1 + min_lat,'int64'); 
    % Centroid Degrees
    centr_x_d(l)  = centr_x(l)/10^7;  
    centr_y_d(l)  = centr_y(l)/10^7; 
    % Centroid in meters
    [min_lon_m min_lat_m centr_x_m centr_y_m ] = ...
    degressToMeters(min_lon,min_lat,centr_x-min_lon,centr_y-min_lat);

    % ---------------------------------------------------------------------
    % Surface aproximation Aproach
    % ---------------------------------------------------------------------

    % Spline aproximation 
    spline          = tpaps([x_p; y_p],snr_1_0_scale(:,l)',0.001);

    % Spline evaluation over points range
    map_values      = fnval(spline, grid_pos);

    % Evaluated spline values normalization
    map_values      = map_values-min(map_values); 
    map_values      = map_values./max(map_values);

    % Heatmap mapping
    map = zeros(map_d, map_d); 
    for i = 1:map_d
        for j = 1:map_d
            map(i, j) = map_values(i+(j-1)*map_d); 
        end
    end 
    
    % Heat map peak Finding
    [aux max_y]     = max(map);
    [aux max_x]     = max(aux);
    max_y           = max_y(max_x);

    % ---------------------------------------------------------------------

    % Max SNR Degree Scale Position:
    max_x_coor(l) 	= cast(del_lon*max_x/map_d + min_lon,'int64'); 
    max_y_coor(l)  	= cast(del_lat*max_y/map_d + min_lat,'int64');
    % Getting max values in meters
    [centr_x_mmin_lon_m min_lat_m max_x_coor_m max_y_coor_m ] = ...
    degressToMeters(min_lon,min_lat,max_x_coor-min_lon,max_y_coor-min_lat);
    % Getting delta values in meters
    [min_lon_m min_lat_m del_lon_m del_lat_m ] = ...
    degressToMeters(min_lon,min_lat,del_lon,del_lat);
    del_lon_m = cast(del_lon_m,FP_PREC); 
    del_lat_m = cast(del_lat_m,FP_PREC); 
    
    % Getting Max Values in Degrees
    max_x_coor_d = max_x_coor/10.^7; 
    max_y_coor_d = max_y_coor/10.^7; 
        
    % ---------------------------------------------------------------------
    % Distance vs SNR finding
    % ---------------------------------------------------------------------
    % Raw SNR vs dist
    if USE_GT
        dist_g_2 = (gps_lat_m-gty_m(l)).^2+(gps_lon_m-gtx_m(l)).^2; 
    else
        dist_g_2 =  (gps_lat_m-max_y_coor_m(l)).^2+...
                    (gps_lon_m-max_x_coor_m(l)).^2;
    end
    if HOR_D
        dist(:,l)   = log10(sqrt(cast(dist_g_2,FP_PREC)));
    else
        dist(:,l)   = log10(sqrt(cast(dist_g_2,FP_PREC)+alt_m^2));
    end
    % Curve fitting:
    pulse_snr_log_sor(:,l)   = sort(pulse_snr_log(:,l)); 
    aux = polyfit(pulse_snr_log(:,l),dist(:,l),POLY_ORDER);
    interp_eval(:,l) = polyval(aux,pulse_snr_log_sor(:,l));
    % ---------------------------------------------------------------------

    % Map image handling and Storing
    map             = flipud(map);
    map_8           = cast(255*map, 'uint8'); 
    map_rgb         = ind2rgb(map_8, jet(255)); 
    map_a           = map-alpha_c_thres/1000;
    map_a           = map_a./max(max(map_a));
    map_a           = cast(255*map_a, 'uint8');
    aux1          	= num2str(curr_run,'%06d');
    aux2          	= num2str(l,'%06d');  
    img_file_name   = strcat(IMG_PREFIX, aux1, '_', aux2, '.png'); 
    imwrite(map_rgb, img_file_name, 'png', 'Alpha', map_a);
end

% -------------------------------------------------------------------------
% Error calc
% -------------------------------------------------------------------------
error       = 10.^interp_eval - 10.^dist;
hist_min    = min(min(error));
hist_max    = max(max(error)); 
hist_del    = (hist_max-hist_min)/HIST_W; 
binranges   = hist_min:hist_del:hist_max;
error_hist  = histc(error, binranges);

% Centroid AVG Error:
avg_c_e = sum(sqrt(cast((centr_y_m-gty_m).^2+...
    (centr_x_m-gtx_m).^2,FP_PREC)))/num_col;
% -------------------------------------------------------------------------

title_prefix = 'Measured SNR over the area for collar #'; 
for l=1:num_col
    aux = 7*(l-1); 
    subplot(3, 7, aux+1:aux+2);
    plot(dist(:,l),pulse_snr_log(:,l),'.b'); 
    hold on
    plot(interp_eval(:,l),pulse_snr_log_sor(:,l), '-r');
    set(gca,'xlim',[0 log10(sqrt(del_lon_m.^2+del_lat_m.^2))]);
    legend('Location','West','Raw Data','PolyFit');
    xlabel('Distance from reference 10^x (m)'); 
    ylabel('Measured SNR 10^x W/W');
    subplot(3, 7,  aux+3); 
    bar(binranges,error_hist(:,l),'histc');
    title('Error Occurrence');
    xlabel('Linear Interp. Error (m)'); 
    ylabel('Number of Occurrences');
    subplot(3, 7,  aux+4:aux+7);
    if SCATTER_W > 0
        scatter(gps_lon_d,gps_lat_d,...
        SCATTER_W,...
            pulse_snr(:,l),'filled'); 
    else
        scatter(gps_lon_d,gps_lat_d,...
        MAX_SCATTER_W*snr_1_0_scale(:,l)+1,...
            pulse_snr(:,l),'filled'); 
    end
    hold on
    if GT_PLOT 
        plot(gtx_d(l), gty_d(l),'*r','MarkerSize',16); 
    end
    plot(max_x_coor_d(l), max_y_coor_d(l),'*g','MarkerSize',16); 
    plot(centr_x_d(l), centr_y_d(l),'*k','MarkerSize',16); 
    hold on
    if GT_PLOT 
        h=legend('Location','NorthWest','SNR','G Truth','Peak','Ctroid');
    else
        h=legend('Location','NorthWest','SNR','Peak','Ctroid');
    end
    set(h,'FontSize',8);
    aux = num2str(l,'%d'); 
    aux = strcat(title_prefix, aux);
    title(aux);
    colorbar;
end

% Metafile storing
aux1          	= num2str(curr_run,'%06d'); 
aux1            = strcat(META_PREFIX, aux1, '.csv');
corners         = [min_lon max_lon min_lat max_lat]; 
points          = [max_x_coor max_y_coor centr_x centr_y]; 
aux2            = [corners; points]; 
dlmwrite(aux1, aux2, 'precision', CSV_NUM_PREC);
fprintf('Process Finished.\n');

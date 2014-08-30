function [rel_lon_d rel_lat_d ] = ...
    metersToDegress( abs_lat, rel_lon_m, rel_lat_m, int_conv )
% Convertion  from meters to geographic degress
    
    % Integer to Double Convertion
    rel_lon_m = cast(rel_lon_m, 'double'); 
    rel_lat_m = cast(rel_lat_m, 'double'); 
    
    % Decimal Point Adjustmeent for the Degree measurements
    abs_lat_d = (cast(abs_lat, 'double')/10000000);
    
    % factor for latitude on longitude
    lon_scale_factor = abs(cosd(abs_lat_d) );  
        
    % Meters to degrees to  convertion
    rel_lon_d = km2deg(rel_lon_m/1000)/lon_scale_factor;
    rel_lat_d = km2deg(rel_lat_m/1000);
    
    % Values scalling
    rel_lon_d = rel_lon_d*10000000;
    rel_lat_d = rel_lat_d*10000000;
    
    % Double to int32 convertion
    if int_conv
        rel_lon_d = cast(rel_lon_d, 'int32'); 
        rel_lat_d = cast(rel_lon_d, 'int32'); 
    end
end
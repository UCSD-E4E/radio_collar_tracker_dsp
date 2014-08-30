function [abs_lon_m abs_lat_m rel_lon_m rel_lat_m ] = ...
    degressToMeters( abs_lon, abs_lat, rel_lon, rel_lat )
% Convertion  from geographic degress to meters
    
    % Decimal Point Adjustmeent for the Degree measurements
    abs_lon_d = (cast(abs_lon, 'double')/10000000);
    abs_lat_d = (cast(abs_lat, 'double')/10000000);
    rel_lon_d = (cast(rel_lon, 'double')/10000000);
    rel_lat_d = (cast(rel_lat, 'double')/10000000);
    
    % Compensating factor for latitude on longitude
    lon_scale_factor = abs(cosd(abs_lat_d) ); 
        
    % Degrees to meters convertion
    abs_lon_m = deg2km(abs_lon_d)*1000;
    abs_lat_m = deg2km(abs_lat_d)*1000;
    rel_lon_m = deg2km(rel_lon_d)*1000*lon_scale_factor;
    rel_lat_m = deg2km(rel_lat_d)*1000;
    
    % Double to int32 convertion
    abs_lon_m = cast(abs_lon_m, 'int32'); 
    abs_lat_m = cast(abs_lat_m, 'int32'); 
    rel_lon_m = cast(rel_lon_m, 'int32'); 
    rel_lat_m = cast(rel_lat_m, 'int32');
end


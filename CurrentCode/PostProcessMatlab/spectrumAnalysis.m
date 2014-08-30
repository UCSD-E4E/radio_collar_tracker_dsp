clear; 
clc; 

% File Parameters
RAW_FILE_PREFIX     = 'RAW_DATA_'; 
META_FILE_PREFIX    = 'RUN_META_'; 
COL_FILE_NAME       = 'COL'; 
JOB_FILE_NAME       = 'JOB'; 
CSV_PREFIX          = 'RUN_'; 
FILE_COUNT_NAME     = 'fileCount'; 
EXTRA_DATA_SIZE     = 13;
MAX_FILES_PROCESS 	= inf; 
CSV_NUM_PREC        = 20;

% Process Parameters
FP_PREC             = 'double';
MAX_SLICING_FACTOR  = 400; 
MIN_SLICING_FACTOR  = 50;  

% Receiver-Specific Parameters
gain_values         = [ 0, 9, 14, 27, 37, 77, 87, 125, 144, 157, 166,...
    197, 207, 229, 254, 280, 297, 328, 338, 364, 372, 386, 402, 421,...
    434, 439, 445, 480, 496 ]; 

% -------------------------------------------------------------------------
% Loading Parameter Files
% -------------------------------------------------------------------------

% Job File Loading
job_file            = fopen(JOB_FILE_NAME); 
raw_data_path       = fgetl(job_file);
aux                 = textscan(job_file, '%s%d'); 
cfg                 = aux{2};
fclose(job_file);

% Job parameters loading
curr_run            = cast(cfg(1),FP_PREC);
freq_em             = cast(cfg(2),FP_PREC); 
pulse_ms            = cast(cfg(3),FP_PREC); 
num_col             = cast(cfg(7),FP_PREC); 
f_drift             = cast(cfg(8),FP_PREC); 

% Collars File Loading
aux                 = csvread(COL_FILE_NAME);
col_f               = aux(:, 1) + f_drift; 

% Meta File Loading
aux_str             = num2str(curr_run,'%06d'); 
cfg_file_path       = strcat(raw_data_path, META_FILE_PREFIX,aux_str);
meta_file           = fopen(cfg_file_path); 
aux                 = textscan(meta_file, '%s%d'); 
cfg                 = aux{2};
fclose(meta_file);

% Meta parameters loading
center_freq         = cast(cfg(1),FP_PREC);
f_samp              = cast(cfg(2),FP_PREC); 
timeout_interrupt   = cast(cfg(3),FP_PREC);
num_fra_p_file      = cast(cfg(6),FP_PREC);
num_files           = cast(cfg(7),FP_PREC);

% Limit number of files to load
if num_files > MAX_FILES_PROCESS
    num_files       = MAX_FILES_PROCESS;  
end

% Derived Parameters Calculation
pul_num_sam         = ceil(f_samp*pulse_ms/1000); 
bin_em              = ceil(pul_num_sam*freq_em/f_samp); 
dem_goal_f          = col_f - center_freq;
data_frame_size     = timeout_interrupt*f_samp/500;
file_frame_size     = data_frame_size +EXTRA_DATA_SIZE;
file_lenght         = file_frame_size*num_fra_p_file;
valid_gain_values   = 10.^(gain_values/100); 
sld_fft_stps        = data_frame_size/2-pul_num_sam;
ttl_n_fr            = num_fra_p_file*num_files;
max_jump            = ceil(MAX_SLICING_FACTOR*pul_num_sam/1000); 
min_jump            = ceil(MIN_SLICING_FACTOR*pul_num_sam/1000);
fft_bin             = ceil(pul_num_sam/2+pul_num_sam*dem_goal_f/f_samp+1);
% -------------------------------------------------------------------------

% Signal Strength Array Memmory Allocation
pulse_snr           = zeros(num_col, ttl_n_fr); 
slice_pos           = zeros(num_col, ttl_n_fr); 
frame_gain          = zeros(ttl_n_fr, 1);  
frame_dc            = zeros(ttl_n_fr, 1); 
gps_pos             = zeros(ttl_n_fr, 3, 'int32'); 

aux_fft_em          = zeros(1, 2*bin_em); 
frame               = zeros(num_fra_p_file, data_frame_size, 'uint8');
gain                = zeros(num_fra_p_file, 1, 'uint8');
measured_f          = zeros(ttl_n_fr,num_col); 

% Looping Thru Each File
for i = 1:num_files 

    % Raw file loading
    aux1            = num2str(curr_run,'%06d'); 
    aux2            = num2str(i,'%06d'); 
    cur_file_name   = strcat(raw_data_path,RAW_FILE_PREFIX,aux1,'_',aux2); 
    fileHandle      = fopen(cur_file_name);
    data            = fread(fileHandle, file_lenght, 'uint8=>uint8')';
    fclose(fileHandle);

    % Loading each frame fron the current raw file
    for j = 1:num_fra_p_file
        
        % Radio data
        aux1 = 1+(j-1)*file_frame_size; 
        aux2 = j*file_frame_size-EXTRA_DATA_SIZE; 
        frame(j, 1:data_frame_size) = data(aux1:aux2);
        
        % Parsing GPS data
        lat = 0; 
        lon = 0; 
        alt = 0; 
        for k = 1:4
            aux1    = j*file_frame_size-EXTRA_DATA_SIZE+k; 
            aux2    = j*file_frame_size-EXTRA_DATA_SIZE+4+k;
            aux3    = j*file_frame_size-EXTRA_DATA_SIZE+8+k; 
            lat     = lat + bitshift(cast(data(aux1),'uint64'),8*(k-1));
            lon     = lon + bitshift(cast(data(aux2),'uint64'),8*(k-1));
            alt     = alt + bitshift(cast(data(aux3),'uint64'),8*(k-1));
        end
        if lat >= 2^31
            lat     = cast(lat, 'int64') - 2^32;  
        end
        if lon >= 2^31
            lon     = cast(lon, 'int64') - 2^32;  
        end
        if alt >= 2^31
            alt     = cast(alt, 'int64') - 2^32;  
        end           
        gps_pos((i-1)*num_fra_p_file+j,1) = lat; 
        gps_pos((i-1)*num_fra_p_file+j,2) = lon;
        gps_pos((i-1)*num_fra_p_file+j,3) = alt;
        
        % Gain Loading
        gain(j)     = data(j*file_frame_size);
    end
    clear data; 
    
    % Looping thru each frame of the current file loaded
    for j = 1:num_fra_p_file
        cur_fr      = j+(i-1)*num_fra_p_file;
        cur_gain    = valid_gain_values(gain(j)+1); 
        
        frame_gain(cur_fr) = cur_gain; 
        
        % Getting complex signal
        in_phase    = (cast(frame(j, 1:2:end), FP_PREC)-128)/128; 
        quadrature  = (cast(frame(j, 2:2:end), FP_PREC)-128)/128; 
        signal_cpx  = complex(in_phase/cur_gain,quadrature/cur_gain); 
                   
        % Removing DC
        signal_cpx  = signal_cpx - mean(signal_cpx); 
        frame_dc(cur_fr) = mean(signal_cpx); 

        % Sliding FFT Process (Finding Point to Crop )
        slc_lkhood  = zeros(num_col, sld_fft_stps); 
        k        	= 1;
        jump        = 1;
        pre_corr  	= 0;
        
        % Process Status Printing
        aux = 'Frame %d/%d Process Running. (%.2f%%)\n'; 
        fprintf(aux,cur_fr,ttl_n_fr,100*cur_fr/ttl_n_fr);
        
        while k < sld_fft_stps
            
            % Slicing
            fft_time_chop = signal_cpx(k:k-1+pul_num_sam);
            
            % Getting FFT of the Slice
            slice_fft   = abs(fft(fft_time_chop))/pul_num_sam;
            aux1        = slice_fft(1:floor(end/2)); 
            aux2        = slice_fft(floor(end/2)+1:end); 
            slice_fft   = [aux2 aux1];
               
            % Getting Strength of the Desired Freq Range
            for l=1:num_col
                aux = slice_fft(floor(fft_bin(l)+(-bin_em:bin_em)));
                slc_lkhood(l, k)  = max(aux);
            end
            
            % Defining next Slice Point
            aux = 0; 
            for l=1:num_col
                if slc_lkhood(l, k) > pre_corr(l)
                    aux = 1; 
                    break; 
                end
            end
            if aux; 
                jump = floor(jump/2); 
            else
                jump = floor(jump*2);             
            end
            pre_corr = slc_lkhood(:, k); 
            if jump > max_jump
                jump = max_jump; 
            elseif jump < min_jump
                jump = min_jump; 
            end
            k = k + ceil(jump); 
        end 
        clear slice_fft; 
        clear aux_fft; 
        clear fft_time_chop; 

        for l=1:num_col
            
            bin = fft_bin(l); 
            
            % Burst Slicing
            [aux max_fft_index] = max(slc_lkhood(l,:));

            % Signal (encountered pulse)
            aux1     	= max_fft_index; 
            aux2    	= max_fft_index+pul_num_sam-1; 
            pulse     	= signal_cpx(aux1:aux2);

            % Signal FFT
            pulse_fft   = fft(pulse)/pul_num_sam;
            aux1      	= pulse_fft(1:floor(end/2)); 
            aux2      	= pulse_fft(floor(end/2)+1:end); 
            pulse_fft 	= [aux2 aux1];

            % Noise 
            aux1        = pulse_fft((bin+bin_em):end); 
            aux2     	= pulse_fft(1:(bin-bin_em-1)); 
            noise    	= [aux2 aux1];

            % Signal
            aux1    	= bin-bin_em;
            aux2    	= bin+bin_em-1; 
            signal   	= pulse_fft(aux1:aux2);

            % Power calculation
            noise_lenght        = pul_num_sam - 2*bin_em; 
            noise_pwr           = sum(abs(noise).^2)/noise_lenght;
            [signal_pwr aux]    = max(abs(signal).^2);
            aux = aux + bin-bin_em; 
            measured_f(cur_fr,l) = aux;
            
            if signal_pwr < 0
                signal_pwr = 0;
            end

            % SNR calculation
            aux                     = signal_pwr/noise_pwr/pul_num_sam; 
            pulse_snr(l, cur_fr)    = 10*log10(aux);
            slice_pos(l, cur_fr)    = max_fft_index; 
        end
    end
    % ---------------------------------------------------------------------
    
    clear frame; 
    clear gain; 
    
end

% Storing results
aux                 = num2str(curr_run,'%06d'); 
csv_filename        = strcat(CSV_PREFIX,aux,'.csv');
csv_aux             = [gps_pos, 1000*pulse_snr'];
dlmwrite(csv_filename, csv_aux, 'precision', CSV_NUM_PREC);

% Storing bin variance data
filename            = strcat('DIST_',aux,'.csv');
aux                 = [fft_bin'; measured_f];
dlmwrite(filename, aux, 'precision', CSV_NUM_PREC);


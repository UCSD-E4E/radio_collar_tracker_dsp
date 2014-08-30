#include <stdlib.h>
#include <math.h> 
#include <string.h> 
#include <stdio.h> 
#include <limits.h>

// File Parameters
#define CSV_PREFIX   	 		"RUN_"
#define META_PREFIX   	 	 	"META_" 
#define JOB_FILE_NAME 	 		"JOB"
#define EXTRA_DATA_SIZE      	13
#define CSV_NUM_PREC         	20
#define SNR_THRES        		0.01
#define STD_STR_SZ  			256

// Process Parameters
#define EARTH_RADIUS         	6371
#define PI 						3.14159265359
#define grau 					1


// Linear Fit buffers
double**    P;
double**    G;
double**    T;
double**    A;
double**    L;
double**    U;
double**    M;
double*     B;
double*     R;
double*     V;

// Aux arrays
char fileBuffer[STD_STR_SZ]; 
char raw_data_path[STD_STR_SZ]; 
char aux_path[STD_STR_SZ]; 

// Aux variables
char charAux 			 	= 0; 
int intAux 					= 0; 
double aux1 				= 0; 
double aux2		 			= 0; 
double aux3					= 0;

// Process Variables
double max_lat		 		= 0; 
double min_lat		 		= 0; 
double del_lat		 		= 0; 

double max_lon		 		= 0; 
double min_lon		 		= 0; 
double del_lon		 		= 0; 

double max_alt		 		= 0; 
double min_alt		 		= 0; 
double del_alt		 		= 0; 
double del_alt_m		 	= 0; 

double* max_snr		 		= 0; 
double* min_snr		 		= 0; 
double* del_snr		 		= 0; 
double* centr_x		 		= 0; 
double* centr_y		 		= 0; 
double* centr_x_m		 	= 0; 
double* centr_y_m	 		= 0; 

// Signal Buffers
double *gps_lat          	= NULL; 
double *gps_lon          	= NULL; 
double *lat_m          		= NULL; 
double *lon_m          		= NULL; 
double *gps_alt         	= NULL; 
double *final_e         	= NULL; 
double **dis_m           	= NULL; 
double **pulse_snr 		  	= NULL;  
double **dis_l10           	= NULL; 
double **snr_l10 		  	= NULL;  
double **snr_1_0_scale 	 	= NULL;  
double **error_m 	 	 	= NULL;  
double **poly 	 			= NULL;  

// File parameters
int curr_run 				= 0; 
int freq_em 				= 0; 
int pulse_ms 				= 0; 
int lin_scale 				= 0; 
int map_d 					= 0; 
int alpha_c_thres 			= 0; 
int num_col 				= 0;
int	ttl_n_fr            	= 0; 

// File Handling Variables
FILE *fileStream;

void init_buffers();

void clean_up_memmory();

void degrees2meters(double abs_lat,double rel_lon,double rel_lat,double *rel_lon_m,double *rel_lat_m);

double** zero_mat(int m, int n); 

void load_files();  

int loadParameter(int comma);

void analysis(); 

// Linear Fit Methods
void preencheMatrizG( double** G, double** P, int m, int n);
double** inicializaMatriz( int m, int n);
double* inicializaVetor( int m);
void zeraMatriz( double** M, int m, int n);
void zeraVetor( double* V, int m);
void preparaMatriz( double** M, double** A, double* B, int m);
void retrosubstituicaoU( double** M, double* R, int m);
void retrosubstituicaoL( double** M, double* R, int m);
void lu( double** A, double** L, double** U, int m);
void resolveSistema( double** A, double** L, double** U, double** M, double* R, double* B);
void transposta( double** G, double** T, int m, int n);
void mutiplicaMatrizM( double** G, double** T, double** A, int m, int n, int o);
void mutiplicaMatrizV( double** P, double** T, double* B, int m, int n);
void zera_buffers();
void linFit_init();
void linFit_cleanup();
void linFit_process();

int main(int argc, char *argv[]){

	printf("Process Started...\n");

	load_files(); 

	analysis(); 

	printf("Process Finished.\n");

	clean_up_memmory(); 
    
    return 0;
}

void init_buffers(){
	centr_x  		= (double*)malloc(num_col*sizeof(double)); 
	centr_y  		= (double*)malloc(num_col*sizeof(double)); 
	centr_x_m  		= (double*)malloc(num_col*sizeof(double)); 
	centr_y_m  		= (double*)malloc(num_col*sizeof(double)); 
	max_snr  		= (double*)malloc(num_col*sizeof(double)); 
	min_snr  		= (double*)malloc(num_col*sizeof(double)); 
	del_snr  		= (double*)malloc(num_col*sizeof(double)); 
	final_e  		= (double*)malloc(num_col*sizeof(double)); 
	gps_lat  		= (double*)malloc(ttl_n_fr*sizeof(double)); 
	gps_lon  		= (double*)malloc(ttl_n_fr*sizeof(double)); 
	lat_m  			= (double*)malloc(ttl_n_fr*sizeof(double)); 
	lon_m  			= (double*)malloc(ttl_n_fr*sizeof(double)); 
	gps_alt  		= (double*)malloc(ttl_n_fr*sizeof(double)); 
	dis_m 			= zero_mat(ttl_n_fr,num_col); 
	pulse_snr 		= zero_mat(ttl_n_fr,num_col);
	dis_l10 	 	= zero_mat(ttl_n_fr,num_col); 
	snr_l10 		= zero_mat(ttl_n_fr,num_col); 
	snr_1_0_scale 	= zero_mat(ttl_n_fr,num_col); 
	error_m 		= zero_mat(ttl_n_fr,num_col); 
	poly 			= zero_mat(2,num_col); 
}

void clean_up_memmory(){
	free(centr_x);
	free(centr_y);
	free(centr_x_m);
	free(centr_y_m);
	free(max_snr);
	free(min_snr);
	free(del_snr);
	free(final_e);
	free(gps_lat);
	free(gps_lon);
	free(lat_m);
	free(lon_m);
	free(dis_m);
	free(gps_alt);
	free(pulse_snr);
	free(dis_l10);
	free(snr_l10);
	free(snr_1_0_scale);
	free(error_m);
	free(poly);
	linFit_cleanup();
}

void degrees2meters(double abs_lat,double rel_lat,double rel_lon,double *rel_lat_m,double *rel_lon_m){

    // Decimal Point Adjustmeent for the Degree measurements
    abs_lat /= pow(10,7);
    rel_lat /= pow(10,7);
    rel_lon /= pow(10,7);
    
    // Compensating factor for latitude on longitude
    double lon_scale_factor = fabs(cos(abs_lat*PI/180));
        
    // Degrees to meters conversion
    rel_lat *= 1000*PI*EARTH_RADIUS/180;
    rel_lon *= 1000*PI*EARTH_RADIUS/180*lon_scale_factor;

    // Results assignment
    *rel_lat_m = rel_lat;
    *rel_lon_m = rel_lon;

}

double** zero_mat(int m, int n){ 

	double **mat; 

	// Memmory Allocation
	mat = (double**)calloc(m,sizeof(double*));
	for (int i = 0; i < m; i++)
		mat[i] = (double*)calloc(n,sizeof(double));

	// Initializing values (0)
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			mat[i][j] = 0;

	return mat;  
}

void load_files(){ 

	// -----------------------------------------------------------
	// JOB file Loading
	// -----------------------------------------------------------
	fileStream 	= fopen(JOB_FILE_NAME, "r");
	// RAW data path loading
	if (fscanf(fileStream, "%s", raw_data_path) == 0) {
        fprintf(stderr, "ERROR Reading Data Config File!\n");
		exit(1);
    }
    // Jumping to the next line of the file
    if (fgets(aux_path, STD_STR_SZ, fileStream) == NULL);
	// Numeric parameters loading
	curr_run 		= loadParameter(1);
	freq_em 		= loadParameter(1);
	pulse_ms 		= loadParameter(1);
	lin_scale 		= loadParameter(1);
	map_d 			= loadParameter(1);
	alpha_c_thres 	= loadParameter(1);
	num_col 		= loadParameter(1);
 	fclose(fileStream);
	// -----------------------------------------------------------
	
	// -----------------------------------------------------------
	// Data file Loading
	// -----------------------------------------------------------
	// Defining CSV file path
	sprintf(aux_path, "%s%06d%s", CSV_PREFIX, curr_run, ".csv");
	fileStream 		= fopen(aux_path, "r");
	// Counting Number of Frames
	ttl_n_fr = 0; 
	do{
		charAux = fgetc(fileStream);
		if(charAux == '\n')
			ttl_n_fr++;
	}while (charAux != EOF);
 	rewind (fileStream);
	init_buffers();
	// Data Parsing
	for (int i = 0; i < ttl_n_fr; i++){
	    if (fgets(fileBuffer, STD_STR_SZ, fileStream) == NULL) {
 			fprintf(stderr, "ERROR Reading Data Config File!\n");
			exit(1);
	    }	    
	    intAux = 0; 
		gps_lat[i] = atoi(fileBuffer+intAux);
	    while(fileBuffer[intAux] != ',' && intAux++ < STD_STR_SZ);
		gps_lon[i] = atoi(fileBuffer+(++intAux));
	    while(fileBuffer[intAux] != ',' && intAux++ < STD_STR_SZ);
		gps_alt[i] = atoi(fileBuffer+(++intAux));
	    while(fileBuffer[intAux] != ',' && intAux++ < STD_STR_SZ);
		for (int j = 0; j < num_col; j++){
			pulse_snr[i][j] = atof(fileBuffer+(++intAux))/1000;
	    	while(fileBuffer[intAux] != ',' && intAux++ < STD_STR_SZ);
		}
	}
 	fclose(fileStream);
	// -----------------------------------------------------------
	linFit_init(); 

}

int loadParameter(int comma){

    if (fgets(fileBuffer, STD_STR_SZ, fileStream) == NULL) {
        fprintf(stderr, "ERROR Reading Data Config File!\n");
		exit(1);
    }
    if (comma){
	    intAux = 0; 
	    while(fileBuffer[intAux] != ':')
	    	intAux++;
	    if(intAux >= STD_STR_SZ){
	        fprintf(stderr, "ERROR Reading Data Config File!\n");
			exit(1);
	    }
	    return atoi(fileBuffer+intAux+1); 
	}
    return atoi(fileBuffer); 
}

void analysis(){

	// MinMaxDelta Calculation
	for (int i = 0; i < num_col; i++){
		min_snr[i] 	= INT_MAX;
		max_snr[i] 	= INT_MIN;
	}
	max_lat 		= INT_MIN;
	max_lon 		= INT_MIN;
	max_alt 		= INT_MIN;
	min_lat 		= INT_MAX;
	min_lon 		= INT_MAX;
	min_alt 		= INT_MAX;
	for (int i = 0; i < ttl_n_fr; i++){
		for (int j = 0; j < num_col; j++){
			// dB to linear SNR conversion
			if (lin_scale){
		 		pulse_snr[i][j]   	= pow(10,pulse_snr[i][j]/10);
		 		snr_l10[i][j]		= log10(pulse_snr[i][j]); 	
			}
		 	// SNR Min/Max
			if (max_snr[j] < pulse_snr[i][j])
				max_snr[j] = pulse_snr[i][j];
			if (min_snr[j] > pulse_snr[i][j])
				min_snr[j] = pulse_snr[i][j];
		}
		// Latitude/Longitude/Altitude Max
		if (max_lat < gps_lat[i])
			max_lat = gps_lat[i];
		if (max_lon < gps_lon[i])
			max_lon = gps_lon[i];
		if (max_alt < gps_alt[i])
			max_alt = gps_alt[i];
		// Latitude/Longitude/Altitude Min
		if (min_lat > gps_lat[i])
			min_lat = gps_lat[i];
		if (min_lon > gps_lon[i])	
			min_lon = gps_lon[i];
		if (min_alt > gps_alt[i])
			min_alt = gps_alt[i];
	}
	// Delta Calculations
	del_lat         = max_lat - min_lat;
	del_lon         = max_lon - min_lon;
	del_alt         = max_alt - min_alt;
	del_alt_m 		= del_alt/pow(10,3);
	for (int i = 0; i < num_col; i++)
		del_snr[i] 	= max_snr[i] - min_snr[i];

	// LatLon Values in meters
	for (int i = 0; i < ttl_n_fr; i++)
		degrees2meters(min_lat,gps_lat[i]-min_lat,gps_lon[i]-min_lon,lat_m+i,lon_m+i);

	// Centroid Calculation
	for (int i = 0; i < num_col; i++){
		aux1 		= 0; 
		aux2 		= 0; 
		aux3 		= 0; 
		for (int j = 0; j < ttl_n_fr; j++){
			// 1-0 scale SNR
    		snr_1_0_scale[j][i]   = (pulse_snr[j][i] - min_snr[i])/del_snr[i];
    		if (snr_1_0_scale[j][i] > SNR_THRES){
	            aux1 += pow(snr_1_0_scale[j][i],2);
	            aux2 += pow(snr_1_0_scale[j][i],2)*(gps_lon[j]-min_lon);
	            aux3 += pow(snr_1_0_scale[j][i],2)*(gps_lat[j]-min_lat);
            }
    	}
		centr_x[i]  = aux2/aux1 + min_lon;  
		centr_y[i]  = aux3/aux1 + min_lat;

		// Centroid conversion to meters
		degrees2meters(min_lat,centr_y[i]-min_lat,centr_x[i]-min_lon,centr_y_m+i,centr_x_m+i);
	}

	// Distance Calculation
	for (int i = 0; i < ttl_n_fr; i++){
		for (int j = 0; j < num_col; j++){
			aux1 			= pow(lon_m[i] - centr_x_m[j],2) + pow(lat_m[i] - centr_y_m[j],2);
			dis_m[i][j] 	= sqrt(aux1 + pow(del_alt_m,2));
	 		dis_l10[i][j] 	= log10(dis_m[i][j]); 
		}
	}

	// LinFit
	for (int j = 0; j < num_col; j++){
		zera_buffers();
		// LinFit buffer filling
		for (int i = 0; i < ttl_n_fr; i++){
			P[i][0] = snr_l10[i][j];
			P[i][1] = dis_l10[i][j];
		}
		linFit_process();
		poly[0][j] = R[0];
		poly[1][j] = R[1];
	}

	// Error Calc
	for (int i = 0; i < num_col; i++){
		final_e[i] = 0;
		for (int j = 0; j < ttl_n_fr; j++){
			aux1 = snr_l10[j][i]*poly[0][i] + poly[1][i];
			error_m[j][i] = fabs(pow(10,aux1) - dis_m[j][i]);
			final_e[i] += error_m[j][i];
		}
		final_e[i] /= ttl_n_fr;
		printf("Error[%d]: %0.2f m\n",i,final_e[i]);
	}

	// -----------------------------------------------------------
	// Storing results
	// -----------------------------------------------------------

	// File Path / Opening file
	sprintf(aux_path, "%s%06d.csv", META_PREFIX, curr_run);
	fileStream = fopen (aux_path, "w+");
   
   	// File writing
	fprintf(fileStream, "%d,%d,%d,%d\n", (int)min_lon, (int)max_lon, (int)min_lat, (int)max_lat);
	for (int i = 0; i < num_col; i++)
		fprintf(fileStream, "%d,%d,%0.2f\n", (int)centr_x[i], (int)centr_y[i],final_e[i]);

	// Closing file
	fclose(fileStream);
	// -----------------------------------------------------------
}




// -----------------------------------------------------------
// Linear Fit Methods
// -----------------------------------------------------------

// Preenche a matriz G usando os pontos em P
void preencheMatrizG( double** G, double** P, int m, int n){
    double aux = 1;
    for (int i = 0; i < m; i++){
        aux = 1;
        for (int j = n; j >= 0; j--){
            G[i][j] = aux;
            aux *= P[i][0];
        }
    }
}

// Aloca espalo de memória para a matriz
double** inicializaMatriz( int m, int n){
    double** M;

    M = (double**)calloc(m, sizeof(double*));

    for( int i = 0; i < m; i++) {
    	M[i] = (double*)calloc(n, sizeof(double));
    }
    return M;
}

// Aloca espalo de memória para o vetor
double* inicializaVetor( int m){
    double* V;

    V = (double*)calloc(m, sizeof(double));

    return V;
}

// Atribui valor 0 a todos os elementos de M
void zeraMatriz( double** M, int m, int n){
    for( int i = 0; i < m; i++)
        for( int j = 0; j < n; j++)
            M[i][j] = 0;
}

// Atribui valor 0 a todos os elementos de V
void zeraVetor( double* V, int m){
    for( int i = 0; i < m; i++)
        V[i] = 0;
}

// Copia o conteúdo da matriz A e do vetor B para a matriz M
void preparaMatriz( double** M, double** A, double* B, int m){
    for (int i = 0; i < m; i++){
        for (int j = 0; j < m; j++)
            M[i][j] = A[i][j];
        M[i][m] = B[i];
    }
}

void retrosubstituicaoU( double** M, double* R, int m){

    // Este loop Sobe a matriz escalonada achando o valor de cada variável
    for (int i = m-1; i >= 0; i--){

        // Preenche o vetor R com B
        R[i] = M[i][m];

        // Subtrai de R todos os valores ponderados entre B e o pivô
        for (int j = (m-1); j > i; j--)
            R[i] -= M[i][j]*R[j];

        // Divide R pelo pivô
        R[i] /= M[i][i];
    }

}

void retrosubstituicaoL( double** M, double* R, int m){

    // Este loop Sobe a matriz escalonada achando o valor de cada variável
    for (int i = 0; i < m; i++){

        // Preenche o vetor R com B
        R[i] = M[i][m];

        // Subtrai de R todos os valores ponderados entre B e o pivô
        for (int j = 0; j < i; j++)
            R[i] -= M[i][j]*R[j];

        // Divide R pelo pivô
        R[i] /= M[i][i];
    }

}

void lu( double** A, double** L, double** U, int m){

    // Zera os elementos de L e U triangularizando-as. Preenche com 1 os elementos da diagonal de L
    for (int i = 0; i < m; i++){
        for (int j = 0; j < m; j++){
            if( i > j)
                U[i][j] = 0;
            else if(i < j)
                L[i][j] = 0;
            else
                L[i][j] = 1;
        }
    }

    double soma = 0; // Variável auxiliar que armazena os valores a serem subtraídos a cada passo

    // A cada passada deste laço, define-se uma linha de U e uma coluna de L
    for (int i = 0; i < m; i++){

        // A cada passada deste laço, define-se uma linha de U
        for (int j = i; j < m; j++){
            soma = 0;
            for (int k = 0; k < i; k++)
                soma += L[i][k]*U[k][j];
            U[i][j] = A[i][j] - soma;
        }

        // A cada passada deste laço, define-se uma coluna de L
        for (int j = i+1; j < m; j++){
            soma = 0;
            for (int k = 0; k < j; k++)
                soma += L[j][k]*U[k][i];
            L[j][i] = (A[j][i] - soma)/U[i][i];
        }

    }
}


void resolveSistema( double** A, double** L, double** U, double** M, double* R, double* B){
    lu( A, L, U, grau+1);
    preparaMatriz( M, L, B, grau+1);
    retrosubstituicaoL( M, R, grau+1);
    preparaMatriz( M, U, R, grau+1);
    retrosubstituicaoU( M, R, grau+1);
}

// Faz a transposta de G em T
void transposta( double** G, double** T, int m, int n){
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            T[j][i] = G[i][j];
}

// Multiplica G por T
void mutiplicaMatrizM( double** G, double** T, double** A, int m, int n, int o){
    double soma = 0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++){
            soma = 0;
            for (int k = 0; k < o; k++)
                soma += G[i][k] * T[k][j];
            A[i][j] = soma;
        }
}

// Multiplica T por yP
void mutiplicaMatrizV( double** P, double** T, double* B, int m, int n){
    for (int i = 0; i < n; i++){
        B[i] = 0;
        for (int j = 0; j < m; j++)
            B[i] += T[i][j] * P[j][1];
    }
}

void zera_buffers(){

    zeraMatriz( P, ttl_n_fr, 2);
    zeraMatriz( G, ttl_n_fr, grau+1);
    zeraMatriz( T, grau+1, ttl_n_fr);
    zeraMatriz( A, grau+1, grau+1);
    zeraMatriz( L, grau+1, grau+1);
    zeraMatriz( U, grau+1, grau+1);
    zeraMatriz( M, grau+1, grau+1+1);

    zeraVetor( B, grau+1);
    zeraVetor( R, grau+1);
    zeraVetor( V, grau+1);

}

void linFit_init(){

    P = inicializaMatriz( ttl_n_fr, 2);
    G = inicializaMatriz( ttl_n_fr, grau+1);
    T = inicializaMatriz( grau+1, ttl_n_fr);
    A = inicializaMatriz( grau+1, grau+1);
    L = inicializaMatriz( grau+1, grau+1);
    U = inicializaMatriz( grau+1, grau+1);
    M = inicializaMatriz( grau+1, grau+1+1);

    B = inicializaVetor( grau+1);
    R = inicializaVetor( grau+1);
    V = inicializaVetor( grau+1);

    zera_buffers();


}

void linFit_cleanup(){
    free(P);
    free(G);
    free(T);
    free(A);
    free(L);
    free(U);
    free(M);
    free(B);
    free(R);
    free(V);
}

void linFit_process(){

    // Preenche matriz G com Px
    preencheMatrizG( G, P, ttl_n_fr, grau);

    // Obtém a transposta de G em T
    transposta(G, T, ttl_n_fr, grau+1);

    // Multiplica a trasposta T por G e jogo em A
    mutiplicaMatrizM(T, G, A, grau+1, grau+1, ttl_n_fr);

    // Multiplica a trasposta T por Py e jogo em B
    mutiplicaMatrizV( P, T, B, ttl_n_fr, grau+1);

    resolveSistema(A, L, U, M, R, B);
}

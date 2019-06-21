//Project 7B - OpenMP trials - By Armand Parajon

#include <cstdlib>
#include <omp.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <limits>
#include <cfloat>

#define SIZE 32768
float array[2 * SIZE];
float sums[SIZE];

void autoCorrelate(float *array, float *sums, int size, int threads, std::string filename);

int main() {

#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not available\n");
    return 1;
#endif

    FILE *fp = fopen( "signal.txt", "r" );
    if( fp == NULL )
    {
        fprintf( stderr, "Cannot open file 'signal.txt'\n" );
        exit( 1 );
    }

    int size;
    //fscanf(fp, "%d", &size);
    size = SIZE;

    //Allocate memory
    //float *array = new float[2 * size];
    //float *sums  = new float[size];

    for( int i = 0; i < size; i++ )
    {
        fscanf( fp, "%f", &array[i] );
        array[i+size] = array[i];		// duplicate the array
    }
    
    fclose(fp);

    //Run with 1 thread
    autoCorrelate(array, sums, size, 1, "openMP_onethread.csv");
    //Run with 8 threads
    autoCorrelate(array, sums, size, 8, "openMP_8threads.csv");

    //Free memory
    //free(array);
    //free(sums);

    return 0;
}

//Use OpenMP to run autocorrelation for thread count and write to file
void autoCorrelate(float *array, float *sums, int size, int threads, std::string filename) {

    omp_set_num_threads(threads);

    int trials = 15;
    double avgTime = 0;
    double peakTime = DBL_MAX;

    for (int r = 0; r < trials; r++) {
        
        double tStart = omp_get_wtime();

        #pragma omp parallel for default(none) shared(size, array, sums)
        for (int shift = 0; shift < size; shift++) {
	        float sum = 0.;
	        for (int i = 0; i < size; i++) {
		        sum += array[i] * array[i + shift];
	        }      
	        sums[shift] = sum;
        }

        double tEnd = omp_get_wtime();

        if (tEnd - tStart < peakTime) {
            peakTime = tEnd - tStart;
        }
          
        avgTime += (tEnd - tStart);   
    }

    avgTime /= trials;
    
    //Write results to file
    FILE *fp = fopen(filename.c_str(), "w"); 

    //Output performance results

    fprintf(fp, "Threads,Peak Performance (Mega AutoCors Per Second),"
                "Average Performance (Mega AutoCors Per Second)\n");
    fprintf(fp, "%d,%lf,%lf\n", threads, (double)size * size / peakTime / 1000000,
                (double)size * size / avgTime / 1000000);

    //Output sum results  
    fprintf(fp, "Shift,Sum\n");
    for (int i = 0; i < size; i++) {
        fprintf(fp, "%d,%f\n", i, sums[i]);
    }    
    fclose(fp);
}

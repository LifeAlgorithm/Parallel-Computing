//Project 7B - SIMD trials - By Armand Parajon

#include <cstdlib>
#include <omp.h>
#include <iostream>
#include <cstdio>
#include "simd.p5.h"
#include <limits>
#include <cfloat>

#define SIZE 32768
float array[2 * SIZE];
float sums[SIZE];

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

    for (int i = 0; i < size; i++ )
    {
        fscanf( fp, "%f", &array[i] );
        array[i+size] = array[i];		// duplicate the array
    }
    fclose(fp);

    int trials = 15;
    double avgTime = 0;
    double peakTime = DBL_MAX;

	for (int i = 0; i < trials; i++) {

		double tStart = omp_get_wtime();

        for (int shift = 0; shift < size; shift++) {
		    sums[shift] = SimdMulSum( &array[0], &array[0+shift], size);
        }
		
        double tEnd = omp_get_wtime();
		
		avgTime += tEnd - tStart;

		if (tEnd - tStart < peakTime) {
            peakTime = tEnd - tStart; 
        }
	}

    avgTime /= trials;

    //Output results to file

    fp = fopen("simd_results.csv", "w");

    fprintf(fp, "Peak Performance (Mega AutoCors Per Second),"
                "Average Performance (Mega AutoCors Per Second)\n");
    fprintf(fp, "%lf,%lf\n", (double)size * size / peakTime / 1000000,
                (double)size * size / avgTime / 1000000);
    
    fprintf(fp, "Shift,Sum\n");
    for (int i = 0; i < size; i++) {
        fprintf(fp, "%d,%f\n", i, sums[i]);
    }
     
    fclose(fp);
    return 0;
}

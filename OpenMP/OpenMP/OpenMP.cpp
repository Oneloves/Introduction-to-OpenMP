// OpenMP.cpp : Définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <omp.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
#include <windows.h>  
#include "ppm_lib.h"
#include "constant.h"

#define PATH "C:/Users/jeanf/Desktop/gare_parallelisme2.ppm"
#define PATH_OUT_QUESTION_A "C:/Users/jeanf/Desktop/mon_imageA.ppm"
#define PATH_OUT_QUESTION_B "C:/Users/jeanf/Desktop/mon_imageB.ppm"


void applyFilterOpenMP(PPMImage *in, PPMImage *out, int filter[], int div);
void applyFilter(PPMImage *in, PPMImage *out, int filter[], int div);


int main(int argc, char* argv[]) {

	PPMImage *imageIn;
	imageIn = readPPM(PATH);

	// Question a
	printf("Filtre sequentiel\n");
	PPMImage *imageOut;
	imageOut = readPPM(PATH);
	applyFilter(imageIn, imageOut, HorizontalBlur, HorizontalBlurDiv);
	writePPM(PATH_OUT_QUESTION_A, imageOut);

	// Question b
	printf("Filtre avec OpenMP\n");
	PPMImage *imageOutOpenMp;
	imageOutOpenMp = readPPM(PATH);
	applyFilterOpenMP(imageIn, imageOutOpenMp, HorizontalBlur, HorizontalBlurDiv);
	writePPM(PATH_OUT_QUESTION_B, imageOutOpenMp);


}


void applyFilterOpenMP(PPMImage *in, PPMImage *out, int filter[], int div) {
	int width;
	int height;
	int x, y, x2, y2;

	double startTime = omp_get_wtime();
	#pragma omp parallel shared(in) private(width, height, x, y, x2, y2)
	{
		width = in->x;
		height = in->y;

		#pragma omp for schedule(dynamic)
		for (y = 2; y < height - 2; y++) {
			for (x = 2; x < width - 2; x++) {

				int gridCounter = 0;
				int finalR = 0;
				int finalG = 0;
				int finalB = 0;

				for (y2 = -2; y2 <= 2; y2++) {
					for (x2 = -2; x2 <= 2; x2++) {
						int index = (y + y2)*width + (x + x2);
						finalR += in->data[index].red * filter[gridCounter];
						finalG += in->data[index].green * filter[gridCounter];
						finalB += in->data[index].blue * filter[gridCounter];
						gridCounter++;
					}
				}
				finalR /= div;
				finalG /= div;
				finalB /= div;
				int index = y*width + x;
				out->data[index].red = finalR;
				out->data[index].green = finalG;
				out->data[index].blue = finalB;
			}
		}
	}
	double endTime = omp_get_wtime();
	double time = endTime - startTime;
	printf("temps : %lf\n\n", time);
}


void applyFilter(PPMImage *in, PPMImage *out, int filter[], int div) {
	int x, y, x2, y2;
	int width = in->x;

	clock_t start = clock();
	for (y = 2; y<in->y - 2; y++) {
		for (x = 2; x<in->x - 2; x++) {
			int gridCounter = 0;
			int finalR = 0;
			int finalG = 0;
			int finalB = 0;
			for (y2 = -2; y2 <= 2; y2++) {
				for (x2 = -2; x2 <= 2; x2++) {
					finalR += in->data[(y + y2)*width + (x + x2)].red * filter[gridCounter];
					finalG += in->data[(y + y2)*width + (x + x2)].green * filter[gridCounter];
					finalB += in->data[(y + y2)*width + (x + x2)].blue * filter[gridCounter];
					gridCounter++;
				}
			}
			finalR /= div;
			finalG /= div;
			finalB /= div;
			out->data[y*width + x].red = finalR;
			out->data[y*width + x].green = finalG;
			out->data[y*width + x].blue = finalB;
		}
	}
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Temps : %f\n\n", seconds);
}
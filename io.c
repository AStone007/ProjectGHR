#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "io.h"

/*.........................................................
 *...test whether you can access this code from main.c.....
 *.........................................................*/
int access_test(int a, int b) {
    int c = a + b;
    return c;
}

/*.........................................................
 *........assign memory for samples in csv file............
 *.........................................................*/
WaveformSample* assign_memory(int num_samples) {

    WaveformSample *data = malloc(num_samples * sizeof(WaveformSample));
                                //num_samples defined later
    if (data == NULL) {printf("ERROR: Trouble allocating memory for WaveformSample\n");}
    else {printf("PROGRESS: Memory space allocated (Waveform sample)\n");}
                                //to handle errors elegantly

    return data; //return as only one result needed
}
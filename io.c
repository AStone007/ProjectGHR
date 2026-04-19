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
                            //use pointer as values will change
WaveformSample* assign_memory(int num_samples) {

    WaveformSample *data = malloc(num_samples * sizeof(WaveformSample));
                            //num_samples defined later
    if (data == NULL) {printf("ERROR: Trouble allocating memory for WaveformSample\n");}
    else {printf("PROGRESS: Memory space allocated (Waveform sample)\n");}
                            //to handle errors elegantly

    return data; //return as only one result needed
}

/*.........................................................
 *...........assign memory for organisation................
 *.........................................................*/
                            //to help with directory handling
Directory* allocate_directory(int num_files) {

    Directory *dir = malloc(sizeof(Directory));
                            //assign and get the address of memory for Directory and stores address in dir
    dir->files = malloc(num_files * sizeof(File));
    dir->file_count = 0; //initialize

    //check for allocation failure
    if (dir == NULL) {
        printf("ERROR: Trouble allocating memory for Directory\n");
        return NULL;}
    else {printf("PROGRESS: Memory space allocated (Directory)\n");}

    if (dir->files == NULL) {
        printf("ERROR: Trouble allocating memory for File array\n");
        free(dir);
        return NULL;}
    else {printf("PROGRESS: Memory space allocated (File array)\n")}

    return dir;
}

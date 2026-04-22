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
    else {printf("PROGRESS: Memory space allocated (File array)\n");}

    return dir;
}

/*.........................................................
 *.....................load directory.....................
 *.........................................................*/
Directory* load_directory(const char *path) {
                            //const ensures no accidental change to path
    char temp_path[256];
    strcpy(temp_path, path);//use temp path as path cannot be manipulated
    DIR *dir = opendir(temp_path);

    //if directory finding fails, get new directory
    while (dir == NULL)
    {
        printf("ERROR: Trouble opening directory\n");
        printf("Enter directory path: ");
        scanf("%255s", temp_path);
        dir = opendir(temp_path);
    }

    //count csv files
    struct dirent *entry;
    int file_counter = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".csv") != NULL)
            {file_counter++;}
    }

    //allocate directory structure
    Directory *directory = allocate_directory(file_counter);
    if (directory == NULL) {closedir(dir); return NULL;}

    //store path
    strcpy(directory->path, temp_path);

    //load data, record file names
    rewinddir(dir); //to go through the file again to load data
    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".csv") != NULL)
        {
            File *current = &directory->files[directory->file_count];
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", temp_path, entry->d_name);
            current->samples = load_file(full_path, &current->num_samples);
            if (current->samples != NULL)
            {
                strcpy(current->filename, entry->d_name);
                directory->file_count++;
            }
        }
    }
    closedir(dir);

    return directory;
}

/*.........................................................
 *................Load each csv file.......................
 *.........................................................*/
WaveformSample* load_file(const char *filepath, int *num_samples) {

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filepath);
        return NULL;
    }

    int rows = 1000;
    WaveformSample *data = assign_memory(rows);
    if (data == NULL) {fclose(file); return NULL;}

    char line[MAX_LINE_LENGTH];
    fgets(line, sizeof(line), file);

    int i = 0;
    while (fgets(line, sizeof(line), file) && i < rows)
    {if (parse_line(line, &data[i])) {i++;}} //function defined later. if line parse success, move to next line
    *num_samples = i;
    fclose(file);
    return data;
}

/*.........................................................
 *.................parse data from csv file................
 *.........................................................*/
int parse_line(char *line, WaveformSample *sample) {

    char *token;
    int field = 0;
    token = strtok(line, ",");
    while (token != NULL && field < 8) { //while not at end of columns or rows
        double value = strtod(token, NULL);
        switch (field) { //to control which column is "written" to
            case 0: sample->timestamp = value; break;
            case 1: sample->phase_A_voltage = value; break;
            case 2: sample->phase_B_voltage = value; break;
            case 3: sample->phase_C_voltage = value; break;
            case 4: sample->line_current = value; break;
            case 5: sample->frequency = value; break;
            case 6: sample->power_factor = value; break;
            case 7: sample->thd_percent = value; break;
        }
        token = strtok(NULL, ",");
        field++;
    }
    return (field == 8);
}
//INCLUDES

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "io.h"
#include "wf.h"


//FORWARD DECLARATIONS

void scan_directory_recursive(const char *base_path, Directory *directory);





//FUNCTIONS


/*.........................................................
 *........assign memory for samples in csv file............
 *.........................................................*/
WaveformSample* assign_memory(int num_samples) {

    WaveformSample *data = malloc(num_samples * sizeof(WaveformSample));
                            //num_samples defined later

    //handle errors gracefully, carry on throughout code
    if (data == NULL) {
        printf("ERROR: Trouble allocating memory for WaveformSample\n");
    }
    else {
        printf(".\n");
    }
    return data; //return as only one result needed
}



/*.........................................................
 *...........assign memory for organisation................
 *.........................................................*/
                            //to help with directory handling
Directory* allocate_directory(int num_files) {

        //Use malloc rather than calloc as values are assigned straight away,
        //as a result initialization not needed and just takes up time.
    //Assign memory for Directory, get the address and store in dir
    Directory *dir = malloc(sizeof(Directory));

    //assign memory for files in directory
    dir->files = malloc(num_files * sizeof(File)); //keep as num_files if there was more time a function
    dir->file_count = 0; //initialize                   //to calculate num_samples could be written but as
                                                        //num_samples declared in brief (1000), code unnecessary
    //malloc failure
    if (dir == NULL) {
        printf("ERROR: Trouble allocating memory for Directory\n");
        return NULL;}
    else {printf(".\n");}
    if (dir->files == NULL) {
        printf("ERROR: Trouble allocating memory for File array\n");
        free(dir);
        return NULL;}
    else {printf(".\n");}

    return dir;
}

/*.........................................................
 *.....................load directory.....................
 *.........................................................*/
Directory* load_directory(const char *path) {
                            //const ensures no accidental change to path

    char temp_path[256];
    //use a buffer variable as path cannot be manipulated
    strcpy(temp_path, path);
    //create pointer to temporary path string
    DIR *dir = opendir(temp_path);

    // when initial (no directory) fails, do not call error
    if (dir == NULL) {
        printf("Enter directory path: ");
        scanf("%255s", temp_path);
        dir = opendir(temp_path);

        //call error then redefine dir through input
        while (dir == NULL) {
            printf("ERROR: Trouble opening directory\n");
            printf("Enter directory path: ");
            scanf("%255s", temp_path);
            dir = opendir(temp_path);
        }
    }

    //count csv files for recursion

    Directory *directory = allocate_directory(1000); // safe upper limit
    if (directory == NULL) {
        closedir(dir);
        return NULL;
    }
    strcpy(directory->path, temp_path);
    // recursively find and load ALL csv files
    scan_directory_recursive(temp_path, directory);

    closedir(dir);
    return directory;
}

void scan_directory_recursive(const char *base_path, Directory *directory) {

    DIR *dir = opendir(base_path);
    if (dir == NULL) return;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // skip . (same folder) and .. (parent folder) to avoid infinite recursion
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[512];    //add /d_name to end of base_path to create file directory inside folder
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry->d_name);
        // check whether it is a folder
        DIR *subdir = opendir(full_path);


        if (subdir != NULL) {  //if it is not, close and recurse
            closedir(subdir);
            scan_directory_recursive(full_path, directory); // !!!RECURSE!!! - creates a loop with the constraint
        }  //                  of the if statement


        else {
            int len = strlen(entry->d_name);    //if there is .csv at end of directory name
            if (len > 4 && strcmp(entry->d_name + len - 4, ".csv") == 0) {

                File *current = &directory->files[directory->file_count]; //cycle through files with count
                strcpy(current->filepath, full_path);
                current->samples = load_file(full_path, &current->num_samples); //cycle through samples
                if (current->samples != NULL) { //if there are samples, copy folder directory to filemane
                    strcpy(current->filename, entry->d_name);
                    directory->file_count++;
                }
            }
        }
    }

    closedir(dir);
}




/*.........................................................
 *................Load each csv file.......................
 *.........................................................*/
WaveformSample* load_file(const char *filepath, int *num_samples) {

    FILE *file = fopen(filepath, "r"); // read and store filepath as pointer "file
    if (file == NULL) { //if NULL error, display message rather than crash
        printf("Error: Cannot open file %s\n", filepath);
        return NULL;
    }

    int rows = 1000;
    WaveformSample *data = assign_memory(rows); //assign memory for the data
    if (data == NULL) {
        fclose(file);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    fgets(line, sizeof(line), file); //load line
    int i = 0;
    while (fgets(line, sizeof(line), file) && i < rows) {
        if (parse_line(line, &data[i])) {
            i++;
        }
    } //function defined later. if parse_line works, move to next line
    *num_samples = i; //update num_samples
    fclose(file);
    return data;
}

/*.........................................................
 *.................parse data from csv file................
 *.........................................................*/
int parse_line(char *line, WaveformSample *sample) {

    char *token;
    int field = 0;
    token = strtok(line, ","); //token becomes the value and the comma
    while (token != NULL && field < 8) { //while token has a value and thd_percent for that line has not been stored,
                                         //store whatever number is in token in value, then store number in value in
                                         //correct value inside sample struct
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
        token = strtok(NULL, ","); //once value stored, reset token to NULL
        field++; //move to next case
    }
    return (field == 8);
}

/*...................................................
  .............write results to file.................
 *...................................................*/

void write_report(File *file, const char *base_path) {

    //create results folder and corresponding path
    char base_name[256];
    snprintf(base_name, sizeof(base_name), "%s", file->filename);
    char *dot = strrchr(base_name, '.');
    if (dot != NULL) {
        *dot = '\0'; //change "." in file directory to \0 as C does not read past it leading to not read .csv
    }

    char folder[512];
    strcpy(folder, file->filepath);

    char *last_slash = strrchr(folder, '/'); //look for slash
    if (last_slash != NULL) { //if there is a slash, change it to a \0
        *last_slash = '\0';
    }

    char result_filename[512]; //make name of full file directory for all the associated results files
    snprintf(result_filename, sizeof(result_filename), "%s/results/%s_results.txt", folder, base_name);

    char results_folder[512];
    snprintf(results_folder, sizeof(results_folder), "%s/results", folder);
    mkdir(results_folder); //now actually create folder for results

    //printf("Writing to: %s\n", result_filename); //to help with debugging remove "//" to activate again

    FILE *out = fopen(result_filename, "w"); //open results file
    if (!out) { //if there is file struggles to open, handle gently
        printf("Error opening output file\n");
        return;
    }

    double limit = 324.9; //store clipping result in clip
    ClippingResult clip = count_clipped(file, limit);


    //process each phase
    for (Phase p = PHASE_A; p <= PHASE_C; p++) {

        //display phase name
        const char *phase_name;
        if (p == PHASE_A) {
            phase_name = "PHASE A";
        }
        else if (p == PHASE_B) {
            phase_name = "PHASE B";
        }
        else if (p == PHASE_C) {
            phase_name = "PHASE C";
        }
        fprintf(out, "%s\n", phase_name);


        //calculate metrics
        double rms = compute_rms(file, p);
        double p2p = compute_peak_to_peak(file, p);
        double dc  = compute_dc_offset(file, p);
        double std = compute_std_dev(file, p);
        WaveformSample **sorted = sort_by_voltage(file, p); //use two "*" as Waveform *samples is data and just
                                                            //need addresses of the data for sorting
        int count;
        double *times;
        if (p == PHASE_A) {
            count = clip.count_A;
            times = clip.times_A;
        } else if (p == PHASE_B) {
            count = clip.count_B;
            times = clip.times_B;
        } else {
            count = clip.count_C;
            times = clip.times_C;
        }


        uint8_t *status; //status points to 8 bit unsigned integer

        //whatever phase is, status is the same
        if (p == PHASE_A) {
            status = &file->status_A;
        }
        else if (p == PHASE_B) {
            status = &file->status_B;
        }
        else {
            status = &file->status_C;
        }

        // reset flags so that recorded errors are not passed on
        *status = 0;
        // set RMS fail flag
        if (!check_compliance(rms)) {
            *status |= RMS_FAIL;
        }
        // set clipping flag
        if (count > 0) {
            *status |= CLIPPING;
        }


        //display phase-wise metrics
        fprintf(out, "\tRMS:\t\t\t%.5f\n", rms);
        fprintf(out, "\tPeak-to-Peak:\t\t%.5f\n", p2p);
        fprintf(out, "\tDC Offset:\t\t%.5f\n", dc);
        fprintf(out, "\tStandard Deviation:\t%.5f\n", std);
        fprintf(out, "\tVoltage Compliance:\t%d\n\n", check_compliance(rms));

        fprintf(out, "\tClipping:\t\t%d out of %d samples\n", count, file->num_samples);
        fprintf(out, "\tOccurred at times:\t");
        if (count == 0) { //if there is no clipping display NONE
            fprintf(out, "NONE\n\n");
        }
        else { //if there is clipping, display times
            for (int i = 0; i < count; i++) {
                fprintf(out, "%.4f", times[i]);
                if (i < count - 1) {
                    fprintf(out, ", ");
                }
            }
            fprintf(out, "\n\n");
        }

        fprintf(out, "\tStatus:\t\t\t"); //display if there is CLIPPING or an RMS fail
        if (*status == 0) {
            fprintf(out, "OK");
        } else {
            if (*status & RMS_FAIL)
                fprintf(out, "RMS_FAIL ");
            if (*status & CLIPPING)
                fprintf(out, "CLIPPING ");
        }
        fprintf(out, "\n");

        fprintf(out, "\tTop 5 |V| values:\t"); //display top 5 voltages
        if (sorted != NULL) {
            int n = file->num_samples;
            for (int i = n - 1; i >= n - 5 && i >= 0; i--) {
                double v = get_voltage(*sorted[i], p);
                fprintf(out, "%.3f", fabs(v));
                if (i > n - 5 && i > 0)
                    fprintf(out, ", ");
            }
        }
        fprintf(out, "\n");
        fprintf(out, "\n\n");
        free(sorted);
    }


    //display whole file metrics
    fprintf(out, "COMBINED DATA\n");
    fprintf(out, "\tPower Factor:\t\t%.5f\n", power_factor(file));
    fprintf(out, "\tTHD (%%):\t\t%.5f\n", thd_percent(file));
    fprintf(out, "\tFrequency (Hz):\t\t%.5f\n", frequency(file));


    fclose(out);
}



/*................................................
 *.........clean memory...........................
 *................................................*/

void free_directory(Directory *dir) {

    if (dir == NULL) return;
    for (int i = 0; i < dir->file_count; i++) {
        free(dir->files[i].samples);
    }
    free(dir->files);
    free(dir);
}
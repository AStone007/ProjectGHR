#ifndef CODINGPROJECT_IO_H
#define CODINGPROJECT_IO_H
#define MAX_LINE_LENGTH 256 // buffer size for each line (not number of rows)

// define structs

typedef struct {
    double timestamp;
    double phase_A_voltage;
    double phase_B_voltage;
    double phase_C_voltage;
    double line_current;
    double frequency;
    double power_factor;
    double thd_percent;
} WaveformSample;

typedef struct {
    WaveformSample *samples;
    int num_samples;
    char filename[256];
} File;

typedef struct {
    File *files;
    int file_count;
} Directory;

// declare functions for use in main.c
int access_test(int a, int b);

WaveformSample* assign_memory(int num_samples);
void get_directory_input(char *input_directory);
Directory* load_directory(const char *path);
WaveformSample* load_file(const char *filepath, int *num_samples);
int parse_line(char *line, WaveformSample *sample);
void write_result(File *file, const char *variable_name, double value);
void free_directory(Directory *dir);

#endif //CODINGPROJECT_IO_H//
// Created by HP on 09/04/2026.
//


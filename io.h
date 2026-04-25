#ifndef CODINGPROJECT_IO_H
#define CODINGPROJECT_IO_H

#include <stdio.h>
#include <dirent.h>

#define MAX_LINE_LENGTH 256

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
    char filepath[512];
} File;

typedef struct {
    char path[256];
    File *files;
    int file_count;
} Directory;

// ✅ MOVE ClippingResult HERE (VERY IMPORTANT)
typedef struct {
    int count_A;
    int count_B;
    int count_C;
    double times_A[1000];
    double times_B[1000];
    double times_C[1000];
} ClippingResult;

int access_test(int a, int b);

WaveformSample* assign_memory(int num_samples);
Directory* allocate_directory(int num_files);
Directory* load_directory(const char *path);
WaveformSample* load_file(const char *filepath, int *num_samples);
int parse_line(char *line, WaveformSample *sample);

void write_result(File *file, const char *variable_name, double value);
void write_clipping_result(File *file, ClippingResult result);

void free_directory(Directory *dir);

#endif
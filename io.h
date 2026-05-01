//DEFINES

#ifndef CODINGPROJECT_IO_H
#define CODINGPROJECT_IO_H

#define MAX_LINE_LENGTH 256

//for bit flags
#include <stdint.h>
#define CLIPPING   (1 << 0)   // 00000001 (bit 0 refers to clipping)
#define RMS_FAIL   (1 << 1)   // 00000010 (bit 1 refers to rms)

//STRUCTS

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
    uint8_t status_A;
    uint8_t status_B;
    uint8_t status_C;
} File;

typedef struct {
    char path[256];
    File *files;
    int file_count;
} Directory;

typedef struct {
    int count_A;
    int count_B;
    int count_C;
    double times_A[1000];
    double times_B[1000];
    double times_C[1000];
} ClippingResult;



//FUNCTIONS

WaveformSample* load_file(const char *filepath, int *num_samples);
Directory* load_directory(const char *path);
Directory* allocate_directory(int num_files);
WaveformSample* assign_memory(int num_samples);
int parse_line(char *line, WaveformSample *sample);
void write_report(File *file, const char *base_path);
void free_directory(Directory *dir);





#endif
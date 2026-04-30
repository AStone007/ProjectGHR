//INCLUDES & DEFINES & MACROS

#ifndef CODINGPROJECT_WF_H
#define CODINGPROJECT_WF_H

#include "io.h"





//STRUCTS

typedef enum {
    PHASE_A,
    PHASE_B,
    PHASE_C
} Phase;



//FUNCTIONS

double compute_rms(File *file, Phase phase);
double compute_peak_to_peak(File *file, Phase phase);
double compute_dc_offset(File *file, Phase phase);
double compute_std_dev(File *file, Phase phase);

int check_compliance(double rms);
double power_factor(File *file);
double thd_percent(File *file);
double frequency(File *file);

ClippingResult count_clipped(File *file, double limit);





#endif
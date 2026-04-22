//
// Created by HP on 09/04/2026.
//

#ifndef CODINGPROJECT_WF_H
#define CODINGPROJECT_WF_H
#include "io.h"

//define phase struct
typedef enum {
    PHASE_A,
    PHASE_B,
    PHASE_C
} Phase;

//declare calculation functions
double rms(File *file, Phase phase);
double amplitude(File *file, Phase phase);
double dc_offset(File *file, Phase phase);
double std_dev(File *file, Phase phase);
int clip_count(File *file, Phase phase, double limit);
int compliance(double rms);


#endif //CODINGPROJECT_WF_H

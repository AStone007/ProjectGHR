#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wf.h"

double get_voltage(WaveformSample sample, Phase phase) {

    switch (phase)
    {
        case PHASE_A: return sample.phase_A_voltage;
        case PHASE_B: return sample.phase_B_voltage;
        case PHASE_C: return sample.phase_C_voltage;
        default: return 0.0;
    }
}

// RMS
double compute_rms(File *file, Phase phase) {

    if (!file || file->num_samples == 0) return 0.0;
    double sum_square = 0.0;
    for (int i = 0; i < file->num_samples; i++) {
        double v = get_voltage(file->samples[i], phase);
        sum_square += v * v;
    }
    return sqrt(sum_square / file->num_samples);
}

// Peak-to-peak
double compute_peak_to_peak(File *file, Phase phase) {

    if (!file || file->num_samples == 0) return 0.0;
    double max = get_voltage(file->samples[0], phase);
                            //max or min may not be 0
    double min = max;
    for (int i = 1; i < file->num_samples; i++) {
        double v = get_voltage(file->samples[i], phase);
        if (v > max) max = v;
        if (v < min) min = v;
    }
    return max - min;
}

//dc offset
double compute_dc_offset(File *file, Phase phase) {

    if (!file || file->num_samples == 0) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < file->num_samples; i++) {
        sum += get_voltage(file->samples[i], phase);
    }
    return sum / file->num_samples;
}

//std dev
double compute_std_dev(File *file, Phase phase) {

    if (!file || file->num_samples == 0) return 0.0;
    double mean = compute_dc_offset(file, phase);
    double sum = 0.0;
    for (int i = 0; i < file->num_samples; i++) {
        double v = get_voltage(file->samples[i], phase);
        sum += (v - mean) * (v - mean);
    }
    return sqrt(sum / file->num_samples);
}

//clipping
ClippingResult count_clipped(File *file, double limit) {
    ClippingResult result = {0};
    for (int i = 0; i < file->num_samples; i++) {
        double t = file->samples[i].timestamp;
        double Va = get_voltage(file->samples[i], PHASE_A);
        double Vb = get_voltage(file->samples[i], PHASE_B);
        double Vc = get_voltage(file->samples[i], PHASE_C);
        if (fabs(Va) >= limit) {
            result.times_A[result.count_A++] = t;
        }
        if (fabs(Vb) >= limit) {
            result.times_B[result.count_B++] = t;
        }
        if (fabs(Vc) >= limit) {
            result.times_C[result.count_C++] = t;
        }
    }
    return result;
}

//compliance
int check_compliance(double rms) {

    return (rms >= 207.0 && rms <= 253.0);
}

// Power factor (read + mean)
double power_factor(File *file) {

    double sum = 0.0;
    for (int i = 0; i < file->num_samples; i++) {
        sum += file->samples[i].power_factor;
    }
    return sum / file->num_samples;
}

// THD mean
double thd_percent(File *file) {

    double sum = 0.0;
    for (int i = 0; i < file->num_samples; i++) {
        sum += file->samples[i].thd_percent;
    }
    return sum / file->num_samples;
}

// Frequency mean
double frequency(File *file) {

    double sum = 0.0;
    for (int i = 0; i < file->num_samples; i++) {
        sum += file->samples[i].frequency;
    }
    return sum / file->num_samples;
}
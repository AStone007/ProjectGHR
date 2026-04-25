#include <stdio.h>
#include "io.h"
#include "wf.h"

int main(void) {

    //load directory of CSV files
    Directory *dir = load_directory("./data");
    if (dir == NULL) {
        printf("Error: Failed to load directory\n");
        return 1;
    }
    //loop through each file
    for (int i = 0; i < dir->file_count; i++) {
        File *file = &dir->files[i];
        printf("Processing file: %s\n", file->filename);
        //phase calculations
        for (Phase p = PHASE_A; p <= PHASE_C; p++) {
            double rms = compute_rms(file, p);
            double p2p = compute_peak_to_peak(file, p);
            double dc = compute_dc_offset(file, p);
            double std = compute_std_dev(file, p);
            //write results
            write_result(file, "RMS", rms);
            write_result(file, "Peak-to-Peak", p2p);
            write_result(file, "DC Offset", dc);
            write_result(file, "Standard Deviation", std);
            //compliance check
            int compliant = check_compliance(rms);
            write_result(file, "Voltage Compliance", compliant);
        }
        // non-repetitive calculations
        double pf = power_factor(file);
        double thd = thd_percent(file);
        double freq = frequency(file);
        write_result(file, "Power Factor", pf);
        write_result(file, "THD (%)", thd);
        write_result(file, "Frequency (Hz)", freq);
        //clipping
        double limit = 325.0; // typical peak for 230V RMS
        ClippingResult clip = count_clipped(file, limit);
        write_clipping_result(file, clip);
    }
    //clear memory
    free_directory(dir);
    printf("complete.\n");

    return 0;
}
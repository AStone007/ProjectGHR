#include <stdio.h>
#include "io.h" //io.h is linked to io.c, io.c links in wf.h, wf.h is included in wf.c
                //this therefore covers all the codes

int main(void) {

    //load directory of CSV files
    Directory *dir = load_directory("./data");
    if (dir == NULL) {
        printf("Error: Failed to load directory\n");
        return 1;
    }

    FILE *umbrella_folder = fopen("master_results.txt", "w");
    if (umbrella_folder == NULL) {
        printf("Error: Could not create master results file\n");
        return 1;
    }
    //compute and write report
    for (int i = 0; i < dir->file_count; i++) {
        File *file = &dir->files[i];
        //printf("Processing file: %s\n", file->filename); //for debugging
        write_report(file, dir->path);
    }
    if (dir->file_count == 0) {
        printf("No valid CSV files were processed.\n");
    }
    fclose(umbrella_folder);
    //clear memory
    free_directory(dir);
    printf("complete.\n");

    return 0;
}
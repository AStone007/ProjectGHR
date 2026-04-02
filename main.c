#include <stdio.h>

int main(void) {
    char[] fileinput;
    printf("Enter file name or desitination:/n");
    scanf("%s", fileinput);
    FILE *fp = fopen(fileinput, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    fclose(fp);
    return 0;
}

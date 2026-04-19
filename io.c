#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "io.h"

/*.........................................................
 *...test whether you can access this code from main.c.....
 *.........................................................*/
int access_test(int a, int b) {
    int c = a + b;
    return c;
}
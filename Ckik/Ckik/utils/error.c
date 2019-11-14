#include<stdio.h>
#include<stdlib.h>
void Ckik_error(char* funcname, char* filename, unsigned long line) {
    fprintf(stderr, "Ckik: ERROR @ %s():%s:%ld\n", funcname, filename, line);
}

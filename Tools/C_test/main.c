#include <stdio.h>
#include <stdlib.h>
#include "../../Firmware/Sound/compressor.h"

compressor_t cmp;

int main(int args, char *argv[]) {

    compressorInit(&cmp, atoi(argv[1]), atoi(argv[2]));

    char *line = NULL;
    size_t size;
    int sample;
    int output;

    while (getline(&line, &size, stdin) != -1) {
        sample = atoi(line);
        output = compressorApply(&cmp, sample);
        printf("%d\n", output);
    }
}

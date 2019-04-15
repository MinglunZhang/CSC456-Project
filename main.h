#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <string>
// We use round number 10 for AES 128
#define ROUND 10
// The bytes of every message
#define MAX_WIDTH 16

extern "C" {
    int encrption(int num_of_lines, unsigned char *data, unsigned char *k);
    int encrption_fpga(int num_of_lines, unsigned char *data, unsigned char *k);
    int decrption(int num_of_lines, unsigned char *data, unsigned char *k);
    int decrption_fpga(int num_of_lines, unsigned char *data, unsigned char *k);
}
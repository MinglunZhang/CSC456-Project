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
#include <cstring>
extern "C" {
	int encryption_fpga(int num_of_lines, unsigned char *data, unsigned char *k);
	int decryption_fpga(int num_of_lines, unsigned char *data, unsigned char *k);
}
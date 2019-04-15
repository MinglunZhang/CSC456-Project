#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char** argv)
{ 
	FILE *fp;
	int mode = 0;
	int numberOfLines;
	int size;
	int num_bytes_read;
	unsigned char *message, *encrypted;
	if(argc < 3)
    {
    	fprintf(stderr,"Usage: %s input_file number_of_lines mode\n",argv[0]);        
    }
    numberOfLines = atoi(argv[2]);
    mode = atoi(argv[3]);
    fp = fopen(argv[1],"r");
    if(fp == NULL) {
    	fprintf(stderr,"Cannot open %s\n",argv[1]);
		exit(EXIT_FAILURE);
    }
    size = numberOfLines * sizeof(unsigned char) * 16;
    message = (unsigned char*)malloc(size);
    num_bytes_read = fread(message,sizeof(unsigned char),numberOfLines * 16,fp);
    fclose(fp);

    for(int i = 0; i < numberOfLines; i++){
    	for(int j = 0; j < 16; j++){
    		printf("%u", message[i*16 + j]);
    	}
    	printf("\n");
    }
    free(message);
    exit(0);

}
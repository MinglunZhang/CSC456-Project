extern "C" {
	int encryption_fpga(int num_of_lines, unsigned char *data, unsigned char *k);
	int decryption_fpga(int num_of_lines, unsigned char *data, unsigned char *k);
}
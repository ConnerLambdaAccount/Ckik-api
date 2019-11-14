#include<time.h>
int read_ids(char* device_id, char* android_id);
int write_ids(char* device_id, char* android_id);

int handle_ids(char* device_id, char* android_id) {
	if(read_ids(device_id, android_id) == 0) {
		return 0;
	}
	if(write_ids(device_id, android_id) == 0) {
		return 0;
	}
	return -1;
}

int read_ids(char* device_id, char* android_id) {
	// example filename: ".exampleusername-ids"
	char dotfile[64];
	snprintf(dotfile, 64, ".%s-ids", Ckik_username);
	FILE* file = fopen(dotfile, "r");

	// File couldn't be opened for reading
	if(file == NULL) {
		return -1;
	}

	// File exists and is open, fgets reads n-1
	fgets(device_id, 33, file);
	fseek(file, 32, SEEK_SET);
	fgets(android_id, 17, file);
	device_id[32] = '\0';
	android_id[16] = '\0';
	
	// Close and return
	fclose(file);
	return 0;
}

void generate_ids(char* device_id, char* android_id) {
	char* hexchars = "abcdef0123456789";
	srand(time(NULL));
	for(int i=0;i<32;i++) {
    	device_id[i] = hexchars[rand() % 16];
    }
	for(int i=0;i<16;i++) {
		android_id[i] = hexchars[rand() % 16];
	}
}

int write_ids(char* device_id, char* android_id) {
	// example filename: ".exampleusername-ids"
	char dotfile[64];
	snprintf(dotfile, 64, ".%s-ids", Ckik_username);
	FILE* file = fopen(dotfile, "w");

	// File couldn't be opened for writing
	if(file == NULL) {
		return -1;
	}
	
	// Generate new ids
	generate_ids(device_id, android_id);

	// Write them to file
	fwrite(device_id, 32, 1, file);
	fwrite(android_id, 16, 1, file);
	device_id[32] = '\0';
	android_id[16] = '\0';

	// Close and return
	fclose(file);
	return 0;
}

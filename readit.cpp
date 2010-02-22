/*
 * Only 16 bit PCM MONO supported now.
 */
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#define BUFFERSIZE 1 // Number of sample rates
#define MINWAVEFREQ 20 // Minimum frequency (assume <40Hz do not exist here)

using namespace std;
bool check_sample(const char*, const char*);
void process_headers(const char*);
void read_cfg(const char*);
void fatal (const char* );

char errmsg[200]; // Temporary message
FILE *in;
int SAMPLE_RATE, // 
	DATA_SIZE,  // Data chunk size in bytes
	PROC_SAMPLES; // Minimum number of samples to process and be sure two waves included

map<string, string> cfg; // OK, ok... Is it possible to store any value but string (Rrrr...) here?

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fatal ("Unspecified config file. exiting\n");
    }
	read_cfg(argv[1]);
    process_headers(cfg["filename"].c_str());
	istringstream i(cfg["treshold1"]);
	double treshold1;
	i >> treshold1;
	// ------------------------------------------------------------
	// This "15" means we have 16 bits for signed, this means
	// 15 bits for unsigned int.
	int min_silence = (int)((1 << 15) * treshold1); // Below this value we have "silence, pshhh..."

    short int buf [SAMPLE_RATE * BUFFERSIZE]; // Process buffer every second
	int head = 0, // Assume sample started
		found_s = 0; // Number of silence samples

	int first_silence = 0;

	printf ("samplerate: %d\ntreshold: %s\n", SAMPLE_RATE, cfg["treshold1"].c_str());
    while (! feof(in) ) {
        fread(buf, 2, SAMPLE_RATE * BUFFERSIZE, in);
        for (int i = 0; i < SAMPLE_RATE * BUFFERSIZE; i++) {
			int cur = abs(buf[i]);

			if (cur <= min_silence) {
				found_s++;
			} else {
				if (found_s >= PROC_SAMPLES) {
					   printf ("Found long silence. Length: %4.4f s, pos: %4.4f - %4.4f\n", 
							(float)found_s/SAMPLE_RATE, (float)(head-found_s)/SAMPLE_RATE, (float)head/SAMPLE_RATE);
					printf("%d;%d\n", head, found_s);
				}
				found_s = 0;
			}
			head++;
			//printf ("min_silence is %d, but this value is: %d", min_silence, buf[i]);
        }
    }

    exit(0);

}
void read_cfg (const char * filename) {
	ifstream file;
	file.open(filename);
	string line;
	int x;
	while (! file.eof() ) {
		getline(file, line);
		x = line.find(":");
		if (x == -1) break; // Last line, exit
		cfg[line.substr(0,x)] = line.substr(x+2);
	}
}

void process_headers(const char * infile) {
    in = fopen(infile, "rb");
    
    // Read bytes [0-3] and [8-11], they should be "RIFF" and "WAVE" in ASCII
    char header[5];
    fread(header, 1, 4, in);

    char sample[] = "RIFF";
    if (! check_sample(sample, header) ) {
        fatal ("RIFF header not found, exiting\n");
    }
	// Checking for compression code (21'st byte is 01, 22'nd - 00, little-endian notation
	short int tmp[2]; // two-byte integer
	fseek(in, 0x14, 0); // offset = 20 bytes
    fread(&tmp, 2, 2, in); // Reading two two-byte samples (comp. code and no. of channels)
    if ( tmp[0] != 1 ) {
		sprintf(errmsg, "Only PCM(1) supported, compression code given: %d\n", tmp[0]);
		fatal (errmsg);
    }
	// Number of channels must be "MONO"
    if ( tmp[1] != 1 ) {
		sprintf(errmsg, "Only MONO supported, given number of channels: %d\n", tmp[1]);
		fatal (errmsg);
    }
	fread(&SAMPLE_RATE, 2, 1, in); // Single two-byte sample
	//printf ("Sample rate: %d\n", SAMPLE_RATE);
	PROC_SAMPLES = SAMPLE_RATE / MINWAVEFREQ;
	fseek(in, 0x24, 0);
	fread(header, 1, 4, in);
	strcpy(sample, "data");
	if (! check_sample(sample, header)) {
		fatal ("data chunk not found in byte offset=36, file corrupted.");
	}
    // Get data chunk size here
	fread(&DATA_SIZE, 4, 1, in); // Single two-byte sample
    //printf ("Datasize: %d bytes\n", DATA_SIZE);
}

bool check_sample (const char * sample, const char * b) { // My STRCPY.
    for(int i = 0; i < sizeof(sample)-1; i++) {
        if (sample[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void fatal (const char * msg) { // Print error message and exit
    perror (msg);
    exit (1);
}

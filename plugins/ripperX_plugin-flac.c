

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define FLAC_OUTPUT_BUF_LENGTH  160


#define PRINTOUT_INTERVAL       0.5

void 
strip_nulls(char* input, int len) {
	int i;

	for (i = 0; i < len; i++) {
		if (input[i] == '\0') {
			input[i] = ' ';
		}
	}
}

int
flac_read_stat (unsigned int *current) {
	char temp[FLAC_OUTPUT_BUF_LENGTH];
	int bytes_read;
	char *p;
	char *q;

	/* Grab new ouput from 'flac'  */
	bytes_read = read (0, (void *) temp, sizeof (temp));
	if (bytes_read) {
		/* strip_nulls(temp, bytes_read); */
		if (p = (char *)strstr(temp, "wav: ")) {
			/* we found the string we need */
			p += 5;
			if (q = (char *)strchr(p, '%')) {
				*q = '\0';	/* terminate */
				*current = atoi(p);
				return (0);
			}
		}
	}
	return (-1);
}

int
main (int argc, char **argv) {
	unsigned int current = 0;

	while (1) {
		if (flac_read_stat (&current) == 0)
			/* print message in form [P 0.xxxxx]\n */
			printf ("[P %f]\n", (double) current / 100.0 );
		usleep (PRINTOUT_INTERVAL * 1000000);
	}
}

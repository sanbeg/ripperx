

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define GOGO_OUTPUT_BUF_LENGTH  1024


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
gogo_read_stat (unsigned int *current, unsigned int *length)
{
	char temp[GOGO_OUTPUT_BUF_LENGTH];
	int bytes_read;
	/* Grab new ouput from 'gogo'  */
	bytes_read = read (0, (void *) temp, sizeof (temp));
	if (bytes_read) {
		strip_nulls(temp, bytes_read);
		sscanf (temp, "{ %u/ %u}", current, length);
		return (0);
	}
	return (-1);
}

int
main (int argc, char **argv)
{
	unsigned int length = 1;
	unsigned int current = 0;

	while (1) {
		if (gogo_read_stat (&current, &length) == 0)
			/* print message in form [P 0.xxxxx]\n */
			printf ("[P %f]\n", (double) current / (double) length);
		usleep (PRINTOUT_INTERVAL * 1000000);
	}
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define OUTPUT_BUF_LENGTH  4096


#define PRINTOUT_INTERVAL       0.5

int
bladeenc_read_stat (double *current)
{
	char temp[OUTPUT_BUF_LENGTH];
	char string[OUTPUT_BUF_LENGTH];
	int bytes_read;
	/* Grab new ouput from 'bladeenc'  */
	bytes_read = read (0, (void *) temp, sizeof (temp));
	if (bytes_read) {
		sscanf (temp, "Status:    %s", string);
		*current = atof (string) / 100.0;
		return (0);
	}
	return (-1);
}

int
main (int argc, char **argv)
{
	double current = 0;

	while (1) {
		if (bladeenc_read_stat (&current) == 0)
			/* print message in form [P 0.xxxxx]\n */
			printf ("[P %f]\n", current);
		usleep (PRINTOUT_INTERVAL * 1000000);
	}
}

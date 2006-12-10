
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define OUTPUT_BUF_LENGTH 4096
#define OFFSET_LENGTH 22

#define PRINTOUT_INTERVAL       0.5

int
l3enc_read_stat (unsigned int *percent)
{
	char temp[OUTPUT_BUF_LENGTH];
	char *string1;
	int bytes_read;
	/* Grab new ouput from 'l3enc'  */
	bytes_read = read (0, (void *) temp, sizeof (temp));
	/* This is ugly, but then again so is the output of l3enc */
	if (bytes_read > OFFSET_LENGTH) {
		string1 = temp + OFFSET_LENGTH;
		string1[3] = 0;
		*percent = atoi (string1);
		return (0);
	}
	return (-1);
}

int
main (int argc, char **argv)
{
	unsigned int percent = 0;

	while (1) {
		if (l3enc_read_stat (&percent) == 0)
			/* print message in form [P 0.xxxxx]\n */
			printf ("[P %f]\n", ((float) (percent)) / 100.0);
		usleep (PRINTOUT_INTERVAL * 1000000);
	}
}

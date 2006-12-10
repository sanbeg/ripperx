
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

/* plugin for lame v3.50. WILL NOT WORK for 3.13 */

#define OUTPUT_BUF_LENGTH 4096
#define OFFSET_LENGTH 16

#define PRINTOUT_INTERVAL       0.5

int
lame_read_stat (double *current)
{
	char temp[OUTPUT_BUF_LENGTH];
	char string[100];
	char *string1;
	int bytes_read;
	/* Grab new ouput from 'lame'  */
	bytes_read = read (0, (void *) temp, sizeof (temp));
        if (bytes_read > OFFSET_LENGTH) {
                sscanf (&temp[OFFSET_LENGTH], "%s%%)|", string);
                *current = atof(string) / 100.0;
                return (0);
        }
	return (-1);
}

int
main (int argc, char **argv)
{
	double current = 0.0;

	while (1) {
		if (lame_read_stat (&current) == 0)
			/* print message in form [P 0.xxxxx]\n */
			printf ("[P %f]\n", current);
		usleep (PRINTOUT_INTERVAL * 1000000);
	}
}

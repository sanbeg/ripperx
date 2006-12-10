
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>

#define A8HZ_MP3_OUTPUT_BUF_LENGTH  1024
#define A8HZ_MP3_OUTPUT_LINE_LENGTH 32

#define CD_SECTORS_PER_FRAME        (float)1.959

#define PRINTOUT_INTERVAL           0.5

int a8hz_mp3_read_stat (unsigned *current);
int find_a8hz_mp3_output_read_offset (char *buf, int begin, int end);
void print_msg (int length, int current);

int
a8hz_mp3_read_stat (unsigned *current)
{
	static char buf[A8HZ_MP3_OUTPUT_BUF_LENGTH];
	int temp_offset, read_offset;
	ssize_t bytes_read;
	int bytes_avail, count;
	static int prev_bytes_avail;

	ioctl (0, FIONREAD, &bytes_avail);
	if (bytes_avail < 4 * A8HZ_MP3_OUTPUT_LINE_LENGTH) {
		if (bytes_avail == prev_bytes_avail)
			/* nothing available, let's wait */
			return -1;
		else {
			/* Record available bytes, and let's just wait */
			prev_bytes_avail = bytes_avail;
			return -1;
		}
	}
	prev_bytes_avail = -1;

	count = 0;
	do {
		bytes_read = read (0, (void *) buf, sizeof (buf));
		temp_offset = bytes_read - 4 * A8HZ_MP3_OUTPUT_LINE_LENGTH - 1;
		read_offset = find_a8hz_mp3_output_read_offset (buf,
		              temp_offset,
		              sizeof (buf) - 1);
		if (read_offset < 0
			        || read_offset > sizeof (buf) - A8HZ_MP3_OUTPUT_LINE_LENGTH) {
			if (count == 0)
				return -1;
			else
				break;
		}
		sscanf (buf + read_offset, "%u", current);
		/* Convert it to cd sector unit */
		*current *= CD_SECTORS_PER_FRAME;
		count++;
	} while (bytes_read == sizeof (buf));

	return 0;
}

int
find_a8hz_mp3_output_read_offset (char *buf, int begin, int end)
{
	int i;

	i = begin;

	do {
		while (buf[i] != 'F' && i <= end - 5)
			i++;

		if (buf[i + 1] == 'r' && isdigit (buf[i + 11]))
			return i + 5;
		else
			i++;
	} while (i <= end - 5);
	return -1;
}

// print out [P 0.xxxx]\n
void
print_msg (int length, int current)
{
	printf ("[P ");
	printf ("%f]\n", (double) current / (double) length);
}

int
main (int argc, char **argv)
{
	int begin, length;
	int current;

	if (argc != 3) {
		fprintf (stderr, "This is ripperX plugin for 8hz-mp3. Syntax is\n"
		         "ripperX_plugin-8hz-mp3 beginning_sector length_in_sector\n");
		exit (1);
	}

	sscanf (argv[1], "%d", &begin);
	sscanf (argv[2], "%d", &length);

	while (1) {
		if (a8hz_mp3_read_stat (&current) == 0)
			print_msg (length, current);
		usleep (PRINTOUT_INTERVAL * 1000000);
	}
}

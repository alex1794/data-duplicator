#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

char usagestr[] = 
	"Usage: ./mydd [-m mode] [-b blocksize] [-c block count] filename\n";

double elapsed(struct timespec start, struct timespec stop)
{
	return (double)stop.tv_sec - (double)start.tv_sec + 
	     ((double)stop.tv_nsec - (double)start.tv_nsec) * 1e-9;
}

void write_file(char *filename, uint64_t bs, uint64_t count)
{
	struct timespec start;
	struct timespec stop;
	uint64_t filesize = bs * count;
	double wrtime = 0;
	double wrbw = 0;

	char *buf = malloc(sizeof(char) * bs);
	if (!buf) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (uint64_t i = 0; i < bs; ++i)
		buf[i] = rand() % 100;

    	int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (!fd) {
		perror("open fd for writing");
		exit(EXIT_FAILURE);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
	for (uint64_t i = 0; i < count; ++i)
		write(fd, buf, bs);
	
	fsync(fd);
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	wrtime = elapsed(start, stop);
	wrbw = (double)filesize / wrtime;
	printf("%lu %.8lf %.2lf\n", filesize, wrtime, wrbw);

	free(buf);
	close(fd);
}

void read_file(char *filename, uint64_t bs, uint64_t count)
{
	struct timespec start;
	struct timespec stop;
	uint64_t filesize = bs * count;
	double rdtime = 0;
	double rdbw = 0;

	char *buf = malloc(sizeof(char) * bs);
	if (!buf) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	int fd = open(filename, O_RDONLY);
	if (!fd) {
		perror("open fd for reading");
		exit(EXIT_FAILURE);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
	for (uint64_t i = 0; i < count; i++)
        	read(fd, buf, bs);
	
	fsync(fd);
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	rdtime = elapsed(start, stop);
    	rdbw = (double)filesize / rdtime;
	printf("%lu %.8lf %.2lf\n", filesize, rdtime, rdbw);

    	free(buf);
	close(fd);
}    

int main(int argc, char **argv)
{
    	int opt;
	char filename[50];
	uint64_t bs = 512;
    	uint64_t count = 1;
    	char mode = 'w';

	while ((opt = getopt(argc, argv, "m:b:c:")) != -1) {
		switch (opt) {
		case 'm' :
			if(sscanf(optarg, "%c", &mode) != 1)
			{
				fprintf(stderr, "%s: bad mode\n", optarg);
				return 1;
			}
			break;
		case 'b' :
			if(sscanf(optarg, "%lu", &bs) != 1)
			{
				fprintf(stderr, "%s: bad block size\n", optarg);
				return 1;
			}
			break;
		case 'c':
			if(sscanf(optarg, "%lu", &count) != 1)
			{
				fprintf(stderr, "%s: bad number of block", optarg);
				return 1;
			}
			break;
		default:
			fprintf(stderr, usagestr);
			exit(EXIT_FAILURE);
		}
	}

    	argc -= optind;
    	argv += optind;
	
    	if (argc > 0) {
        	if (argc != 1) {
		    	return printf(usagestr), 1;
		}

        	if (sscanf(argv[0], "%s", filename) != 1) {
			fprintf(stderr, "%s: bad filename\n", argv[0]);
			return 1;
		}
   	} else { 
		return printf(usagestr), 1;
	}

	
	printf("%d %lu %lu ", (mode == 'r') ? 0 : 1, bs, count);
	
	srand(getpid());

	if (mode == 'w') {
        	write_file(filename, bs, count);
	} else if (mode == 'r') {
        	read_file(filename, bs, count);
	} else {
        	fprintf(stderr, "Bad mode : read mode (r) and write mode (w)\n");
        	exit(EXIT_FAILURE);
    	}
 
	return 0;
}

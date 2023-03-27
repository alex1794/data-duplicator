#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


double elapsed(struct timespec start, struct timespec stop)
{
    return (double)stop.tv_sec - (double)start.tv_sec + ( (double)stop.tv_nsec - (double)start.tv_nsec )*1e-9;
}

void write_file(char *filename, unsigned int bs, unsigned int count)
{
	struct timespec start, stop;
    double filesize = (double)bs*count*1e-9; // Gigabyte
	char *buf = malloc(sizeof(char) * bs);
    for(int i = 0; i < bs; ++i)
        buf[i] = rand() % 100;	

	int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(!fd)
	{
		perror("open fd");
		exit(EXIT_FAILURE);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for(int i = 0; i < count; ++i)
		write(fd, buf, bs);
	fsync(fd);
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	double wrtime = elapsed(start, stop);
    double wrbw = filesize / wrtime;
	printf("Write of %.2lf GB file : %lf seconds, %lf GB/s\n", filesize, wrtime, wrbw);

    free(buf);
	
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	close(fd);
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
	printf("Close time of %s : %lf secondes\n", filename, elapsed(start, stop));
}

void read_file(char *filename)
{
    printf("Not implemented\n");   
}    

int main(int argc, char **argv)
{
    srand(getpid());

    int opt;
	char filename[50];
	unsigned int bs = 0, count = 0;
    char mode = 0;

	while((opt = getopt(argc, argv, "m:f:b:c:")) != -1)
	{
		switch(opt) {
			case 'm' :
				if(sscanf(optarg, "%c", &mode) != 1)
				{
					fprintf(stderr, "%s: bad mode\n", optarg);
					return 1;
				}
				break;
			case 'f' :
				if(sscanf(optarg, "%s", &filename) != 1)
				{
					fprintf(stderr, "%s: bad filename\n", optarg);
					return 1;
				}
				break;
			case 'b' :
				if(sscanf(optarg, "%u", &bs) != 1)
				{
					fprintf(stderr, "%s: bad block size\n", optarg);
					return 1;
				}
				break;
			case 'c':
				if(sscanf(optarg, "%u", &count) != 1)
				{
					fprintf(stderr, "%s: bad number of block", optarg);
					return 1;
				}
				break;
			default:
				fprintf(stderr, "Usage: %s -m mode -f filepath -b blocksize -c number of block\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if(argc <= 1)
		return printf("Usage: %s -m mode -f filepath -b blocksize -c number of block\n", argv[0]), 1;

	printf("%c - %s - %d - %d\n", mode, filename, bs, count);

    if(mode == 'w')
        write_file(filename, bs, count);
    else if (mode == 'r')
        read_file(filename);
    else
    {
        fprintf(stderr, "Bad mode : read mode (r) and write mode (w)\n");
        exit(EXIT_FAILURE);
    }

	return 0;
}

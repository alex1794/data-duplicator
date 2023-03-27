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

int main(int argc, char **argv)
{
    srand(getpid());

    int opt;
	char filename[50];
	unsigned int bs = 0, count = 0;

	while((opt = getopt(argc, argv, "f:b:c:")) != -1)
	{
		switch(opt) {
			case 'f' :
				if(sscanf(optarg, "%s", &filename) != 1)
				{
					fprintf(stderr, "%s: bad filename", optarg);
					return 1;
				}
				break;
			case 'b' :
				if(sscanf(optarg, "%u", &bs) != 1)
				{
					fprintf(stderr, "%s: bad block size", optarg);
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
				printf("Usage: %s -f filepath -b blocksize -c number of block\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if(argc <= 1)
		return printf("Usage: %s -f filepath -b blocksize -c number of block\n", argv[0]), 1;

	printf("%s - %d - %d\n", filename, bs, count);


	struct timespec start, stop;
    double filesize = (double)bs*count*1e-9;
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
	printf("Write of %.2lfG file : %lf secondes, %lf Go/s\n", filesize, wrtime, wrbw);

    free(buf);
	
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	close(fd);
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
	printf("Close time of %s : %lf secondes\n", filename, elapsed(start, stop));

	return 0;
}

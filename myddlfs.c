#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define ALIGNMENT 4096

struct Param {
	uint64_t bs;
	uint64_t count;
	unsigned int nthread;
	int flags;
	char mode;
	char filename[50];
};

char usagestr[] = 
	"Usage: ./mydd [-m mode] [-d] [-b blocksize] [-c block count] [-w thread count] filename\n";

double elapsed(struct timespec start, struct timespec stop)
{
	return (double)stop.tv_sec - (double)start.tv_sec + 
	     ((double)stop.tv_nsec - (double)start.tv_nsec) * 1e-9;
}

void write_file(char *filename, int flags, uint64_t bs, uint64_t count)
{
	struct timespec start;
	struct timespec stop;
	uint64_t filesize = bs * count;
	double wrtime = 0;
	double wrbw = 0;

	char *buf = aligned_alloc(ALIGNMENT, sizeof(char) * bs);
	if (!buf) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (uint64_t i = 0; i < bs; ++i)
		buf[i] = rand() % 100;

	int fd = open(filename, flags, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		perror("open fd for writing");
		exit(EXIT_FAILURE);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
	for (uint64_t i = 0; i < count; ++i)
		if (write(fd, buf, bs) < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	
	fsync(fd);
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	wrtime = elapsed(start, stop);
	wrbw = (double)filesize / wrtime;
	printf("%lu %.8lf %.2lf\n", filesize, wrtime, wrbw);

	free(buf);
	close(fd);
}

void read_file(char *filename, int flags, uint64_t bs, uint64_t count)
{
	struct timespec start;
	struct timespec stop;
	uint64_t filesize = bs * count;
	double rdtime = 0;
	double rdbw = 0;

	char *buf = aligned_alloc(ALIGNMENT, sizeof(char) * bs);
	if (!buf) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	int fd = open(filename, flags);
	if (fd < 0) {
		perror("open fd for reading");
		exit(EXIT_FAILURE);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
	for (uint64_t i = 0; i < count; i++)
		if (read(fd, buf, bs) < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
	
	fsync(fd);
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	rdtime = elapsed(start, stop);
	rdbw = (double)filesize / rdtime;
	printf("%lu %.8lf %.2lf\n", filesize, rdtime, rdbw);

	free(buf);
	close(fd);
}

void *thread_kernel(void *arg)
{
	struct Param *param = (struct Param *) arg;

	if (param->mode == 'w') {
        	write_file(param->filename, param->flags, param->bs, param->count);
	} else if (param->mode == 'r') {
        	read_file(param->filename, param->flags, param->bs, param->count);
	} else {
        	fprintf(stderr, "Bad mode : read mode (r) and write mode (w)\n");
        	exit(EXIT_FAILURE);
	}

	return NULL;
}

void process_bench(struct Param *param)
{
	pthread_t tid[param->nthread];
	
	srand(getpid());

	for (unsigned int i = 0; i < param->nthread; ++i)
		pthread_create(&tid[i], NULL, thread_kernel, (void *) param);

	for (unsigned int i = 0; i < param->nthread; ++i)
		pthread_join(tid[i], NULL);
}

int main(int argc, char **argv)
{
	int opt;
	
	struct Param param;
	param.bs = 512;
	param.count = 1;
	param.nthread = 1;
	param.flags = O_WRONLY | O_CREAT | O_TRUNC;
	param.mode = 'w';

	while ((opt = getopt(argc, argv, "m:db:c:w:")) != -1) {
		switch (opt) {
		case 'm' :
			if(sscanf(optarg, "%c", &param.mode) != 1)
			{
				fprintf(stderr, "%s: bad mode\n", optarg);
				return 1;
			}

			if (param.mode == 'r')
				param.flags = O_RDONLY;

			break;
		case 'd':
			param.flags |= O_DIRECT;
			break;
		case 'b' :
			if(sscanf(optarg, "%lu", &param.bs) != 1) {
				fprintf(stderr, "%s: bad block size\n", optarg);
				return 1;
			}

			break;
		case 'c':
			if(sscanf(optarg, "%lu", &param.count) != 1) {
				fprintf(stderr, "%s: bad number of block\n", optarg);
				return 1;
			}

			break;
		case 'w':
			if(sscanf(optarg, "%u", &param.nthread) != 1) {
				fprintf(stderr, "%s: bad number of thread\n", optarg);
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

		if (sscanf(argv[0], "%s", param.filename) != 1) {
			fprintf(stderr, "%s: bad filename\n", argv[0]);
			return 1;
		}
	} else {
		return printf(usagestr), 1;
	}

	printf("%d %lu %lu %u\n", (param.mode == 'r') ? 0 : 1, param.bs, param.count, param.nthread);
	
	process_bench(&param);
 
	return 0;
}

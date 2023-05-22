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

struct Thread {
	struct Param param;
	unsigned int num;
	char filename[60];
	pthread_t tid;
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
		if (pwrite64(fd, buf, bs, bs*i) < 0) {
			perror("pwrite64");
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
	struct Thread *thr = (struct Thread *) arg;
	struct Param param = thr->param;

	if (param.mode == 'w') {
        	write_file(thr->filename, param.flags, param.bs, param.count);
	} else if (param.mode == 'r') {
        	read_file(thr->filename, param.flags, param.bs, param.count);
	} else {
        	fprintf(stderr, "Bad mode : read mode (r) and write mode (w)\n");
        	exit(EXIT_FAILURE);
	}

	return NULL;
}

void process_bench(struct Param *param)
{
	struct timespec start, stop;
	double time = 0;
	double bw = 0;
	uint64_t filesize = param->bs * param->count * param->nthread;

	struct Thread *thr = aligned_alloc(128, sizeof(struct Thread) * param->nthread);
	
	srand(getpid());

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	for (unsigned int i = 0; i < param->nthread; ++i) {
		thr[i].param = *param;
		thr[i].num = i;
		snprintf(thr[i].filename, 60, "%s%u", param->filename, i);
		pthread_create(&(thr[i].tid), NULL, thread_kernel, (void *) &thr[i]);
	}

	for (unsigned int i = 0; i < param->nthread; ++i)
		pthread_join(thr[i].tid, NULL);

	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	time = elapsed(start, stop);
	bw = (double) filesize / time;
	printf("\n\n%lu %.8lf %.2lf\n", filesize, time, bw);

	free(thr);
}

int main(int argc, char **argv)
{
	int opt;
	
	struct Param param;
	param.bs = 4096;
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

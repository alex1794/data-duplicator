all: mydd ddbench

mydd: myddlfs.c
	gcc -Wall -Wextra -O2 -march=native $^ -o $@

ddbench: myddlfs.c
	gcc -Wall -Wextra -O2 -march=native -DBENCH $^ -o $@

clean:
	rm -f mydd ddbench

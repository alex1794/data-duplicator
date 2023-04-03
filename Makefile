all: mydd

mydd: myddlfs.c
	gcc -Wall -Wextra -O2 -march=native $^ -o $@

clean:
	rm mydd

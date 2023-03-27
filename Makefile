all: mydd ddbench

mydd: myddlfs.c
	gcc $^ -o $@

ddbench: myddlfs.c
	gcc -DBENCH $^ -o $@

clean:
	rm -f mydd

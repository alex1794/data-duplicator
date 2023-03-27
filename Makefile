all: mydd

mydd: myddlfs.c
	gcc $^ -o $@

clean:
	rm -f mydd

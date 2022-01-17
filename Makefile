CFLAGS=

all:	album

album_utils.o:	album_utils.c album_utils.h
				gcc $(CFLAGS) -c album_utils.c

process.o:		process.c process.h
				gcc $(CFLAGS) -c process.c

album.o:		album.c
				gcc $(CFLAGS) -c album.c

album:			album.o album_utils.o	process.o
				gcc $(CFLAGS) album_utils.o process.o album.c -o album

clean:
	rm *.o album
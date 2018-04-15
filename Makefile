CC = gcc
CFLAGS = -g -Wall -O0 -Werror -Wshadow -Wwrite-strings

huffman: main.o heap.o decode.o encode.o
	$(CC) -o huffman main.o heap.o decode.o encode.o

main.o: main.c heap.h decode.h encode.h
	$(CC) $(CFLAGS) -c main.c

heap.o: heap.c heap.h
	$(CC) $(CFLAGS) -c heap.c

decode.o: decode.c decode.h heap.h
	$(CC) $(CFLAGS) -c decode.c

encode.o: encode.c encode.h heap.h
		$(CC) $(CFLAGS) -c encode.c

clean:
		rm -f *.o huffman
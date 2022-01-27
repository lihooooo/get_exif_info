./test:./main.o ./exif.o
	$(CC) -o./test $^

%.o:%.c
	$(CC) -c -o$@ $^
	




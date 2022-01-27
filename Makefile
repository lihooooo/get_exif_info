./test:./main.o ./dm_exif.o
	$(CC) -o./test $^

%.o:%.c
	$(CC) -c -o$@ $^
	




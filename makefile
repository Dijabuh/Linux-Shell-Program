CC = gcc
TARGET = shell

$(TARGET): $(TARGET).o parser.o file_res.o
	$(CC) $^ -o $@ -std=c99

$(TARGET).o: $(TARGET).c
	$(CC) -c $< -o $@

parser.o: parser.c
	$(CC) -c parser.c -o parser.o

file_res.o: file_res.c
	$(CC) -c file_res.c -o file_res.o -std=c99

clean:
	rm -rf *.o *~ $(TARGET)

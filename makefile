CC = gcc
TARGET = shell

$(TARGET): $(TARGET).o parser.o
	$(CC) $^ -o $@ -std=c99

$(TARGET).o: $(TARGET).c
	$(CC) -c $< -o $@

parser.o: parser.c
	$(CC) -c parser.c -o parser.o

clean:
	rm -rf *.o *~ $(TARGET)

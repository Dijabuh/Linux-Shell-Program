CC = gcc
TARGET = shell
STND = -std=c99

$(TARGET): $(TARGET).o parser.o file_res.o execution.o
	$(CC) $^ -o $@ $(STND)

$(TARGET).o: $(TARGET).c
	$(CC) -c $< -o $@ $(STND)

parser.o: parser.c
	$(CC) -c parser.c -o parser.o $(STND)

file_res.o: file_res.c
	$(CC) -c file_res.c -o file_res.o $(STND)

execution.o: execution.c
	$(CC) -c execution.c -o execution.o $(STND)

clean:
	rm -rf *.o *~ $(TARGET)

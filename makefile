CC = gcc
TARGET = shell
STND = -std=c99

EXE = execution
BUILTIN = builtins

$(TARGET): $(TARGET).o parser.o file_res.o $(EXE).o $(BUILTIN).o
	$(CC) $^ -o $@ $(STND)

$(TARGET).o: $(TARGET).c
	$(CC) -c $< -o $@ $(STND)

parser.o: parser.c
	$(CC) -c parser.c -o parser.o $(STND)

file_res.o: file_res.c
	$(CC) -c file_res.c -o file_res.o $(STND)

$(EXE).o: $(EXE).c
	$(CC) -c $(EXE).c -o $(EXE).o $(STND)

$(BUILTIN).o: $(BUILTIN).c
	$(CC) -c $(BUILTIN).c -o $(BUILTIN).o $(STND)

clean:
	rm -rf *.o *~ $(TARGET)

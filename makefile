CC = gcc
TARGET = shell
STND = -std=c99

EXE = execution
BUILTIN = builtins
REDIRECT = redirection

$(TARGET): $(TARGET).o parser.o file_res.o $(EXE).o $(BUILTIN).o $(REDIRECT).o piping.o background.o
	$(CC) $^ -g -o $@ $(STND)

$(TARGET).o: $(TARGET).c
	$(CC) -c -g $< -o $@ $(STND)

parser.o: parser.c
	$(CC) -c -g parser.c -o parser.o $(STND)

file_res.o: file_res.c
	$(CC) -c -g file_res.c -o file_res.o $(STND)

$(EXE).o: $(EXE).c $(EXE).h
	$(CC) -g  -c $(EXE).c -o $(EXE).o $(STND)

$(BUILTIN).o: $(BUILTIN).c $(BUILTIN).h
	$(CC) -g -c $(BUILTIN).c -o $(BUILTIN).o $(STND)

$(REDIRECT).o: $(REDIRECT).c $(REDIRECT).h
	$(CC) -g -c $(REDIRECT).c -o $(REDIRECT).o $(STND)

piping.o: piping.c
	$(CC) -g -c piping.c -o piping.o $(STND)

background.o: background.c
	$(CC) -g -c background.c -o background.o $(STND)

clean:
	rm -rf *.o *~ $(TARGET)

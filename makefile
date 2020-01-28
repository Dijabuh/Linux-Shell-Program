CC = gcc
TARGET = shell
STND = -std=c99

EXE = execution
BUILTIN = builtins
REDIRECT = redirection
PARSER = parser
FILERES = file_res
PIPE = piping
BGD = background

HEADERS = $(EXE).h $(BUILTIN).h $(REDIRECT).h $(PARSER).h $(FILERES).h $(PIPE).h $(BGD).h
IMPLMNT = $(EXE).c $(BUILTIN).c $(REDIRECT).c $(PARSER).c $(FILERES).c $(PIPE).c $(BGD).c
SUPPORT = README.md makefile

$(TARGET): $(TARGET).o $(PARSER).o $(FILERES).o $(EXE).o $(BUILTIN).o $(REDIRECT).o $(PIPE).o $(BGD).o
	$(CC) $^ -g -o $@ $(STND)

$(TARGET).o: $(TARGET).c
	$(CC) -c -g $< -o $@ $(STND)

$(PARSER).o: $(PARSER).c $(PARSER).h
	$(CC) -c -g $(PARSER).c -o $(PARSER).o $(STND)

$(FILERES).o: $(FILERES).c $(FILERES).h
	$(CC) -c -g $(FILERES).c -o $(FILERES).o $(STND)

$(EXE).o: $(EXE).c $(EXE).h
	$(CC) -g  -c $(EXE).c -o $(EXE).o $(STND)

$(BUILTIN).o: $(BUILTIN).c $(BUILTIN).h
	$(CC) -g -c $(BUILTIN).c -o $(BUILTIN).o $(STND)

$(REDIRECT).o: $(REDIRECT).c $(REDIRECT).h
	$(CC) -g -c $(REDIRECT).c -o $(REDIRECT).o $(STND)

$(PIPE).o: $(PIPE).c $(PIPE).h
	$(CC) -g -c $(PIPE).c -o $(PIPE).o $(STND)

$(BGD).o: $(BGD).c $(BGD).h
	$(CC) -g -c $(BGD).c -o $(BGD).o $(STND)

clean:
	rm -rf *.o *~ $(TARGET)

package:
	rm -rf *.o *~ $(TARGET)
	make
	git log > commit_log.txt
	tar -cvf project1_anderson_brown.tar $(TARGET).c $(HEADERS) $(IMPLMNT) $(SUPPORT) commit_log.txt

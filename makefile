CC = gcc
TARGET = cbrsh

$(TARGET): $(TARGET).o
	$(CC) $< -o $@ -std=c99

$(TARGET).o: $(TARGET).c
	$(CC) -c $< -o $@

clean:
	rm -rf *.o *~ $(TARGET)

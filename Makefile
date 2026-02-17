CC=gcc
CFLAGS=-g -Wall

SOURCES=$(wildcard *.c)
LIB_SOURCES=$(filter-out scanx.c, $(SOURCES))
OBJECTS=$(LIB_SOURCES:.c=.o)

TARGET=scanx

all: $(TARGET)

$(TARGET): $(TARGET).c $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(TARGET).c -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGET)

.PHONY: all clean

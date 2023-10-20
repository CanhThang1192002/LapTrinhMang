CC = gcc
CFLAGS = -Wall
LIBS = -lcurl $(shell xml2-config --libs)
CFLAGS += $(shell xml2-config --cflags)

SOURCES = resolver.c
EXECUTABLE = resolver

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LIBS)

clean:
	rm -f $(EXECUTABLE)

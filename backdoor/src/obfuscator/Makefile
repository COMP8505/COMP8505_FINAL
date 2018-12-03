SOURCES += src/main.c src/msbuffer.c
OUTPUT = bin/bd
CFLAGS += -lpthread -fopenmp -ggdb -lm

all: $(SOURCES)
	cc $(SOURCES) $(CFLAGS) -o $(OUTPUT)

clean: 
	rm -rf bin/*

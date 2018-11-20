SOURCES += src/main.c
OUTPUT = bin/bd
CFLAGS += -lpthread -fopenmp

all: $(SOURCES)
	cc $(SOURCES) $(CFLAGS) -o $(OUTPUT)

clean: 
	rm -rf bin/*
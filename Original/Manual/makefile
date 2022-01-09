CC=g++

CFLAGS=-Wall `pkg-config --cflags opencv` -std=c++11 -c

LDFLAGS=-Wall `pkg-config --libs opencv` -o

all: Man

Man: main.o server.o CVision.o CControlPi.o libSonar.o
	$(CC) $(LDFLAGS) Man main.o server.o CVision.o CControlPi.o libSonar.o -lwiringPi -lpthread -lpigpio -lraspicam -lraspicam_cv
	
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp

CVision.o: CVision.cpp
	$(CC) $(CFLAGS) CVision.cpp 
	
CControlPi.o: CControlPi.cpp
	$(CC) $(CFLAGS) CControlPi.cpp 

libSonar.o: libSonar.cpp
	$(CC) $(CFLAGS) libSonar.cpp
	
clean:
	rm *o Man
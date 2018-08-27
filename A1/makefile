# CS410 makefile
# Bradley Pospeck

CC=g++
OBJS=main.o Model.o Models.o Object3d.o
CFLAGS=-Wall -I. -c -std=c++14
LFLAGS=-Wall

all: modeltoworld
modeltoworld: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o modeltoworld

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Model.o: Model.cpp Model.h
	$(CC) $(CFLAGS) Model.cpp
	
Models.o: Models.cpp Models.h
	$(CC) $(CFLAGS) Models.cpp
	
Object3d.o: Object3d.cpp Object3d.h
	$(CC) $(CFLAGS) Object3d.cpp
	
clean:
	rm -f $(OBJS) *~ modeltoworld

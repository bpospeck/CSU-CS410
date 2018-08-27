# CS410 A4 makefile
# Bradley Pospeck

CC=g++
OBJS= Material.o Object3d.o Model.o Models.o Light.o Image.o Sphere.o main.o
CFLAGS=-O3 -Wall -I. -c -std=c++14
LFLAGS=-Wall

all: raytracer
raytracer: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o raytracer

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Material.o: Material.cpp Material.h
	$(CC) $(CFLAGS) Material.cpp

Model.o: Model.cpp Model.h
	$(CC) $(CFLAGS) Model.cpp
	
Models.o: Models.cpp Models.h
	$(CC) $(CFLAGS) Models.cpp
	
Object3d.o: Object3d.cpp Object3d.h
	$(CC) $(CFLAGS) Object3d.cpp
	
Light.o: Light.cpp Light.h
	$(CC) $(CFLAGS) Light.cpp

Image.o: Image.cpp Image.h
	$(CC) $(CFLAGS) Image.cpp

Sphere.o: Sphere.cpp Sphere.h
	$(CC) $(CFLAGS) Sphere.cpp

clean:
	rm -f $(OBJS) *~ raytracer


CC = gcc
C++ = g++
LIBDIRS = -L/usr/lib64
INCDIRS = -I/usr/include
LDLIBS =  -lglut -lGL -lGLU -lX11 -lm


main: main.o Vec2.o
	$(C++) -o main main.o Vec2.o $(INCDIRS) $(LIBDIRS) $(LDLIBS)

main.o: main.cpp
	$(C++) -c main.cpp

Vec2.o: Vec2.cpp
	$(C++) -c Vec2.cpp

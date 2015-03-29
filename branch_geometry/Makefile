#
#	Makefile for OpenGL example from Chapter 1 in Red Book
#

CC = g++ 
CFLAGS = -D_DEBUG -I ./glm
CPPFLAGS = -g -I ./glm

.cpp.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@  $<

LIBS = -lGL -lGLU -lGLEW -lglut 

SRCS = branch.cpp LoadShaders.cpp
OBJS = branch.o LoadShaders.o

submission_ver1: $(OBJS)
	g++ -g -o branch $(OBJS) $(LIBS)

clean:
	rm -f branch *.o

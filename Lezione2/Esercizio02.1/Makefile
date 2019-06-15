CC = g++
CFLAGS = -Wall -O3 --std=c++11

programma.exe : programma.o random.o
	$(CC) random.o programma.o -o programma.exe
programma.o : programma.cpp
	$(CC) -c programma.cpp -o programma.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o programma.exe seed.out

CC = g++
CFLAGS = -Wall -O3 --std=c++11

programma.exe : programma.o random.o ago.o
	$(CC) random.o programma.o ago.o -o programma.exe
programma.o : programma.cpp
	$(CC) -c programma.cpp -o programma.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
ago.o : ago.cpp ago.h
	$(CC) -c ago.cpp -o ago.o $(CFLAGS)
clean :
	rm *.o programma.exe seed.out

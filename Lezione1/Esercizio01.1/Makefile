CC = g++

programma.exe : programma.o random.o
	$(CC) random.o programma.o -o programma.exe
programma.o : programma.cpp
	$(CC) -c programma.cpp -o programma.o
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o
clean :
	rm *.o programma.exe seed.out

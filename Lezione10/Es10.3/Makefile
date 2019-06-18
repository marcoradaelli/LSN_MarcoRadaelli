CC = mpicxx

programma.exe : programma.o random.o percorso.o selezione.o crossover.o
	$(CC) random.o programma.o percorso.o selezione.o crossover.o -o programma.exe
programma.o : programma.cpp percorso.h citta.h selezione.h crossover.h
	$(CC) -c programma.cpp -o programma.o
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o
percorso.o: percorso.h citta.h percorso.cpp
	$(CC) -c percorso.cpp -o percorso.o
selezione.o: citta.h percorso.h selezione.cpp selezione.h
	$(CC) -c selezione.cpp -o selezione.o
crossover.o: crossover.h crossover.cpp percorso.h
	$(CC) -c crossover.cpp -o crossover.o
clean :
	rm *.o programma.exe seed.out

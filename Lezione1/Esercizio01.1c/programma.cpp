#include "random.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

double calcolaErrore(float * arrMedia, float * arrMedia2, int i);

int main(){
	// Avvio il generatore di numeri casuali.
	Random rnd;
	int seed[4];
	int p1, p2;
	ifstream Primes("Primes");
	if (Primes.is_open()){
	  Primes >> p1 >> p2 ;
	} else cerr << "PROBLEM: Unable to open Primes" << endl;
	Primes.close();

	ifstream input("seed.in");
	string property;
	if (input.is_open()){
	  while ( !input.eof() ){
		 input >> property;
		 if( property == "RANDOMSEED" ){
		    input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
		    rnd.SetRandom(seed,p1,p2);
		 }
	  }
	  input.close();
	} else cerr << "PROBLEM: Unable to open seed.in" << endl;\
	
	// Apro l'output su file.
	ofstream fout;
	fout.open("chisq.dat");
	
	// In quanti intervalli divido [0,1)?
	int M = 100; // L'i-esimo intervallo andrà da 1/M*i a 1/M*(i+1) per i=0,...,M-1.
	
	// Quante volte ripeto l'esperimento?
	int V=100;
	
	// Quanti numeri devo considerare?
	int N = 10000; 
	
	// Questo ciclo permette la riesecuzione dell'esperimento.
	for(int p=0; p<V; p++){
		// Dichiaro un array dinamico di dimensione M, cioè il numero di intervalli considerati.
		int * countPerIntervallo = new int [M];
		// Faccio partire il generatore e registro il numero ottenuto nella corretta cella dell'array.
		double num;

		// Questo ciclo for serve per inizializzare a zero l'array.
		for(int i=0; i<M; i++){
			countPerIntervallo[i]=0;
		}

		// Questo ciclo for serve per far girare i numeri casuali considerati.
		for(int j=0; j<N; j++){
			// Estraggo un numero.
			num=rnd.Rannyu();
			
			// In quale intervallo si trova questo numero?
			for(int i=0; i<M; i++){
				// Se si trova nell'intervallo considerato, aggiungo 1 alla cella dell'array.
				if(num>=1./(double)M*(double)i and num<1./(double)M*(double)(i+1.)){
					countPerIntervallo[i]++;
				}
			}
		}

		// A questo punto calcolo il Chi quadro.
		double expected = (double)N/(double)M; // Quanti punti ci aspettiamo di avere in ogni intervallo.
		// Questo ciclo calcola per accumulo il Chi quadro.
		double chiSq=0;
		for(int i=0; i<M; i++){
			// Per ogni intervallo, calcola la differenza quadrata tra il numero degli eventi reali e quelli attesi.
			chiSq+=pow((countPerIntervallo[i]-expected),2)/expected;
		}

		// Stampo il Chi quadro ottenuto.
		cout<<"Tentativo "<<p<<": Chi = "<<chiSq<<endl<<endl;

		// Stampo l'array ottenuto.
		/*for (int i=0; i<M; i++){
			cout<<countPerIntervallo[i]<<endl;
		}*/

		delete [] countPerIntervallo;
		
		// Stampo su file il Chi quadro ottenuto.
		fout<<p<<"   "<<chiSq<<endl;
	}
	
	// Chiudo l'output su file.
	fout.close();
	
		
	return 0;
}

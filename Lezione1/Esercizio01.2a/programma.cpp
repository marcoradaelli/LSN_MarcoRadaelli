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
	
	// Avvio l'output su file.
	ofstream out_unif;
	ifstream in_N;
	in_N.open("parameters.in"); // Questo file contiene solo il parametro N.
	
	// Sommo N numeri ottenuti con una distribuzione uniforme.
	int N=100;
	in_N>>N;
	// Il tutto viene iterato V volte.
	int V=1e4;
	
	out_unif.open("uniform_data.dat");
	
	// I risultati vengono inseriti in un array di interi.
	int nBin = 100;
	int * countPerBinUnif = new int[nBin]; // Il bin "i" prende i dati da iN/nBin a (i+1)N/nBin, per i=0,...,nBin-1.
	
	// Questo ciclo azzera il count per bin.
	for(int j=0; j<nBin; j++){
		countPerBinUnif[j]=0;
	}
	
	// Questo ciclo for implementa la ripetizione dell'esperimento.
	for(int p=0; p<V; p++){
		// Questo ciclo for esegue la somma.
		double accu_unif=0;
		double x_unif;
		for(int i=0; i<N; i++){
			// Aumento la variabile di accu_unif.
			x_unif=rnd.Rannyu();
			accu_unif+=x_unif;
		}
		
		// A questo punto in accu_unif ho registrato il valore della somma ottenuto. In quale bin si trova?
		for(int j=0; j<nBin; j++){
			if(accu_unif>=(double)j*(double)N/(double)nBin and accu_unif<(double)(j+1)*(double)N/(double)nBin){
				countPerBinUnif[j]++;
			}
		}
	}
	
	// Questo ciclo stampa l'array ottenuto.
	for(int j=0; j<nBin; j++){
		cout<<countPerBinUnif[j]<<endl;
	}
	
	// Questo ciclo salva l'array ottenuto su file. Per ogni intervallo viene registrato il punto medio.
	for(int j=0; j<nBin; j++){
		double punto;
		punto=(double)(j+0.5)*(double)N/(double)nBin;
		out_unif<<punto<<"   "<<countPerBinUnif[j]<<endl;
	}
	
	delete [] countPerBinUnif;
			
	return 0;
}

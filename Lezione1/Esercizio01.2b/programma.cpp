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
	ofstream out_exp;
	ifstream in_N;
	in_N.open("parameters.in");
	// Contiene nell'ordine N, nBin e valMax.
	
	// Sommo N numeri ottenuti con una distribuzione esponenziale.
	int N=100;
	in_N>>N;
	
	// Il tutto viene iterato V volte.
	int V=1e4;
	
	out_exp.open("exponential_data.dat");
	
	// I risultati vengono inseriti in un array di interi.
	int nBin = 100;
	in_N>>nBin;
	int * countPerBinExp = new int[nBin];
	int maxValore =200;
	in_N>>maxValore;
	
	cout<<"Lavoro con N="<<N<<"   nBin="<<nBin<<"   valMax="<<maxValore<<endl;
	
	// Questo ciclo azzera il count per bin.
	for(int j=0; j<nBin; j++){
		countPerBinExp[j]=0;
	}
	
	// Questo ciclo for implementa la ripetizione dell'esperimento.
	for(int p=0; p<V; p++){
		// Questo ciclo for esegue la somma.
		double accu_exp=0;
		double x_exp;
		for(int i=0; i<N; i++){
			// Aumento la variabile di accumulo.
			x_exp=rnd.Exponential(1);
			accu_exp+=x_exp;
		}
		
		// A questo punto in accu_exp ho registrato il valore della somma ottenuto. In quale bin si trova?
		for(int j=0; j<nBin; j++){			
			if(accu_exp>=(double)j*(double)maxValore/(double)nBin and accu_exp<(double)(j+1)*(double)maxValore/(double)nBin){
				countPerBinExp[j]++;
			}
		}
	}

	
	// Questo ciclo salva l'array ottenuto su file. Per ogni intervallo viene registrato il punto medio.
	for(int j=0; j<nBin; j++){
		double punto;
		punto=(double)(j+0.5)*(double)maxValore/(double)nBin;
		out_exp<<punto<<"   "<<countPerBinExp[j]<<endl;
	}
	
	delete [] countPerBinExp;
			
	return 0;
}

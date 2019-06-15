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
	ofstream out_cauchy;
	ifstream in_N;
	
	// Nel file dei parametri ci sono nell'ordine N, nBin, valMax.
	in_N.open("parameters.in");
	
	// Sommo N numeri ottenuti con una distribuzione Cauchy-Lorentz.
	int N=0;
	in_N>>N;
	// Il tutto viene iterato V volte.
	int V=1e4;
	
	out_cauchy.open("cauchy_data.dat");
	
	// I risultati vengono inseriti in un array di interi.
	int nBin=100;
	in_N>>nBin;
	int valMax=1000;
	in_N>>valMax;
	
	cout<<"Eseguo codice con N="<<N<<"   nBin="<<nBin<<"   valMax="<<valMax<<endl;
	
	int * countPerBinCauchy = new int[nBin];	
	
	// Questo ciclo azzera il count per bin.
	for(int j=0; j<nBin; j++){
		countPerBinCauchy[j]=0;
	}
	
	// Questo ciclo for implementa la ripetizione dell'esperimento.
	for(int p=0; p<V; p++){
		// Questo ciclo for esegue la somma.
		double accu_cauchy=0;
		double x_cauchy;
		for(int i=0; i<N; i++){
			// Aumento la variabile di accumulo.
			x_cauchy=rnd.CauchyLorentz(0,1);
			accu_cauchy+=x_cauchy;
		}
		
		// A questo punto in accu_cauchy ho registrato il valore della somma ottenuto. In quale bin si trova?
		for(int j=0; j<nBin; j++){
			if(accu_cauchy>=(double)j*(double)valMax/(double)nBin and accu_cauchy<(double)(j+1)*(double)valMax/(double)nBin){
				countPerBinCauchy[j]++;
			}
		}
	}

	
	// Questo ciclo salva l'array ottenuto su file. Per ogni intervallo viene registrato il punto medio.
	for(int j=0; j<nBin; j++){
		double punto;
		punto=(double)(j+0.5)*(double)valMax/(double)nBin;
		out_cauchy<<punto<<"   "<<countPerBinCauchy[j]<<endl;
	}
	
	delete [] countPerBinCauchy;
			
	return 0;
}

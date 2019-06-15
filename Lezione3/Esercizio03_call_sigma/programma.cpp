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
	
	// Inserisco i parametri.
	double initial_price=100;
	double delivery_time=1;
	double strike_price=100;
	double risk_free_rate=0.1;
	double volatility=0.3;
	
	// File di output.
	ofstream f_out;
	f_out.open("prices.dat");
	
	// Numero di lanci per ogni sigma.
	int L=100000;
	
	// Questo ciclo varia sulle sigma.
	double sigma_init=0.01;
	double sigma_step=0.005;
	int sigma_nsteps = 100;
	for (int s=0; s<sigma_nsteps; s++){
		volatility=sigma_init+sigma_step*s;
		double accu_profit=0;
		for (int j=0; j<L; j++){
			// Lancio una simulazione, cioe' calcolo il prezzo finale.
			double final_price;
			double Z_gaussian=rnd.Gauss(0,1);
			final_price=initial_price * exp((risk_free_rate-0.5*volatility*volatility)*delivery_time + volatility*Z_gaussian*sqrt(delivery_time));
			if (final_price-strike_price>0){
				accu_profit+=exp(-risk_free_rate*delivery_time) * (final_price-strike_price);
			} else {
				accu_profit+=0;
			}
		}

		
		// Stampo su file il prezzo dell'opzione e il valore di sigma.
		f_out<<volatility<<"   "<<accu_profit/(double)L<<endl;
	}
	
	f_out.close();
	
	return 0;
}

double calcolaErrore(float * arrMedia, float * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
//		cout<<"ArrMedia(i)="<<arrMedia[i]<<"    ArrMedia2[i]="<<arrMedia2[i]<<endl;
//		cout<<"Passo "<<i<<", trovato errore "<<sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i)<<endl<<endl;
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

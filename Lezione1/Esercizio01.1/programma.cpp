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
	
	// A questo punto il generatore e' pronto.
	// Numero totale di lanci.
	int M=100000;
	int N=100;
	int L=M/N; // Ricordarsi di inserire M multiplo di N;
	// Creo gli array per salvare i dati.
	float * arrAve = new float[N];
	float * arrAve2 = new float[N];
	
	// Eseguo i cicli.
	for (int i=0; i<N; i++){
		double somma = 0;
		double numero;
		for (int j=0; j<L; j++){
			numero=rnd.Rannyu();
			// Aggiungo il numero all'accumulatore della somma.
			somma+=numero;
		}
		// Alla fine di un blocco calcolo la media e la salvo nella casella dell'array.
		arrAve[i]=somma/(float)L;
		// E il quadrato della media.
		arrAve2[i]=arrAve[i]*arrAve[i];
	}
	
	// A questo punto ho nei due array le medie di tutti i blocchi e le medie quadrate di tutti i blocchi.
	float * progrAve = new float[N];
	float * progrAve2 = new float[N];
	float * errProgr = new float[N];
	// Adesso ciclo sugli array che ho creato accumulando.
	for (int i=0; i<N; i++){
		progrAve[i]=0;
		progrAve2[i]=0;
		for (int j=0; j<i+1; j++){
			progrAve[i]+=arrAve[j];
			progrAve2[i]+=arrAve2[j];
		}
		
		progrAve[i]/=(float)(i+1);
		progrAve2[i]/=(float)(i+1);
		errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
	}

	// Salvo su file per ogni blocco la media e la deviazione standard.
	ofstream fout1;
			
	fout1.open("data.dat");

	for(int i=0; i<N; i++){
		fout1<<i<<"   "<<progrAve[i]<<"   "<<errProgr[i]<<endl;
	}

	delete [] arrAve;
	delete [] arrAve2;
	delete [] progrAve;
	delete [] progrAve2;
	delete [] errProgr;
	
	fout1.close();
	
	cout<<"Esecuzione conclusa"<<endl;

	return 0;
}

double calcolaErrore(float * arrMedia, float * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

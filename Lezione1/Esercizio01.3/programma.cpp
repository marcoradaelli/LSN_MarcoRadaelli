#include "random.h"
#include "ago.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

double calcolaErrore(double * arrMedia, double * arrMedia2, int i);
double calcolaTheta(Random * rnd);

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
	
	// Lancio casualmente 100 aghi. Per il momento ricavo theta da una distribuzione uniforme tra 0 e pi.
	// Ricavo x, y tra 0.5 e 100.
	double passo=2;
	double lunghezza=1;
	
	
		
	// Registro i dati in blocchi.
	int n_blocchi=100;
	int n_prove_per_blocco=1e5;
	
	// Questo array contiene i valori di pi greco calcolati in ogni blocco.
	double * arrValori = new double [n_blocchi];
	double * arrValori2 = new double [n_blocchi];
	
	// Azzero l'array dei valori.
	for(int j=0; j<n_blocchi; j++){
		arrValori[j]=0;
		arrValori2[j]=0;
	}
	
	// Questo ciclo passa tra un blocco e l'altro.
	for(int p=0; p<n_blocchi; p++){
		// Questo ciclo esegue le prove in ciascun blocco.
		int counter=0;
		for(int i=0; i<n_prove_per_blocco; i++){
			// Dichiaro un ago.
			double x=0;
			double y=rnd.Rannyu(0.5,100);
			double theta;
			
			// Adesso devo estrarre l'angolo. Lo faccio in una apposita funzione.
			theta=calcolaTheta(&rnd);
			
			ago needle(x,y,theta,lunghezza);
			if(needle.interseca(passo)){
				counter++;
			} 
		}
		
		cout<<"Blocco "<<p<<". L'ago ha intersecato "<<counter<<" volte su "<<n_prove_per_blocco<<endl; 
		double stima_pi=2*(double)lunghezza*(double)n_prove_per_blocco/(double)counter/(double)passo; 

		cout<<"Quindi la stima di pi greco è "<<stima_pi<<endl<<endl;
		// Salvo la stima.
		arrValori[p]=stima_pi;
		// Salvo anche il quadrato della stima.
		arrValori2[p]=stima_pi*stima_pi;
	}
	
	double * progrAve = new double[n_blocchi];
	double * progrAve2 = new double[n_blocchi];
	double * errProgr = new double[n_blocchi];
	// Adesso ciclo sugli array che ho creato accumulando.
	for (int i=0; i<n_blocchi; i++){
		progrAve[i]=0;
		progrAve2[i]=0;
		for (int j=0; j<i+1; j++){
			progrAve[i]+=arrValori[j];
			progrAve2[i]+=arrValori2[j];
		}
		
		progrAve[i]/=(double)(i+1);
		progrAve2[i]/=(double)(i+1);
//		cout<<"progrAve["<<i<<"]="<<progrAve[i]<<endl;
//		cout<<"progrAve2["<<i<<"]="<<progrAve2[i]<<endl;
		errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
	}
	
	// Carico su file i valori di progrAve e di errProgr.
	ofstream fout;
	fout.open("data.dat");
	for(int i=0; i<n_blocchi; i++){
		fout<<i<<"   "<<progrAve[i]<<"   "<<errProgr[i]<<endl;
	}
	
	delete [] arrValori;
	delete [] arrValori2;
	delete [] progrAve;
	delete [] progrAve2;
	delete [] errProgr;
	
	fout.close();
	
	return 0;
}

double calcolaErrore(double * arrMedia, double * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

double calcolaTheta(Random * rnd){
	// Questa funzione ottiene l'angolo theta. 
	double x,y;
	do{
		x=rnd->Rannyu(0,1);
		y=rnd->Rannyu(0,1);
//		cout<<"Iterazione"<<endl;
	} while (x*x+y*y>=1);
//	cout<<"Fine iterazione"<<endl;
	// Quando finisce questo ciclo ho in (x,y) un punto all'interno della circonf. unitaria.
	// Ne ricavo l'arcotangente.
	return atan(y/x);
}

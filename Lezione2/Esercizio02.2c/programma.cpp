#include "random.h"
#include "walker.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

double calcolaErrore(float * arrMedia, float * arrMedia2, int i);
int scegliDirezione(Random * rnd);
bool scegliVerso(Random * rnd);

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
	
	// Dichiaro un array di walker nella posizione iniziale.
	int quanti_walker=2000;
	int n_passi=100;
	double passo_reticolare = 1.;
	int dimensione_blocco =  100;
	int quanti_blocchi = quanti_walker/dimensione_blocco;
	walker_discreto * wlk = new walker_discreto[quanti_walker];
	ofstream f_out;
	f_out.open("distances.dat");
	ofstream f_out2;
	f_out2.open("positions.dat");
	
	// Questo ciclo cicla sui passi da compiere. 
	for(int p=0; p<n_passi; p++){
	
		cout<<"Passo "<<p<<endl;
		
		float * arrAve = new float[quanti_blocchi];
		float * arrAve2 = new float[quanti_blocchi];
		
		// Suddivido in blocchi i walker. Interrogo prima i primi dimensione_blocco, poi i successivi.
		// Questo ciclo cicla sui blocchi.
		for(int i=0; i<quanti_blocchi; i++){
			double somma=0;
			// All'interno di ogni blocco voglio calcolare la media delle distanze e salvarla in arrAve. Per farlo devo interrogare i walker.
			for(int w=i*dimensione_blocco; w<(i+1)*dimensione_blocco; w++){
				// Chiedo al walker quanto e' lontano dall'origine.
				somma+=pow(wlk[w].distanzaDaOrigine(passo_reticolare),2);
				// Faccio fare al walker un passo.
				wlk[w].passo(scegliDirezione(&rnd),scegliVerso(&rnd));
			}
			// A questo punto calcolo la media del blocco.
			arrAve[i]=somma/(double)dimensione_blocco;
			cout<<"Il blocco "<<i<<" propone una media di "<<arrAve[i]<<endl;
			
			// E la media quadrata del blocco.
			arrAve2[i]=arrAve[i]*arrAve[i];
		}
		
		// Alla fine di questo calcolo nei due array ho il valore delle medie a tutti gli step e il valore delle medie quadrate a tutti gli step.
		// Adesso uso tre array per accumulare.
		float * progrAve = new float[quanti_blocchi];
		float * progrAve2 = new float[quanti_blocchi];
		float * errProgr = new float[quanti_blocchi];
		
		// Calcolo l'errore
		for(int i=0; i<quanti_blocchi; i++){
			progrAve[i]=0;
			progrAve2[i]=0;
			errProgr[i]=0;
			for (int j=0; j<i+1; j++){
				progrAve[i]+=arrAve[j];
				progrAve2[i]+=arrAve2[j];
			}
		
			progrAve[i]/=(float)(i+1);
			progrAve2[i]/=(float)(i+1);
//			cout<<"ProgrAve2["<<i<<"]="<<progrAve2[i]<<endl;
			errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
		}
		
		// Scrivo nel file la media finale e l'errore finale.
		double errPropagato = 0.5/progrAve[quanti_blocchi-1]*errProgr[quanti_blocchi-1];
		// Se e' zero escludo.
		if(p==0){
			errPropagato=0;
		}
		f_out<<p<<"   "<<sqrt(progrAve[quanti_blocchi-1])<<"   "<<errPropagato<<endl;
		
		// Prima della fine del passo distruggo gli array usati per calcolare l'errore.
		delete [] arrAve;
		delete [] arrAve2;
		delete [] progrAve;
		delete [] progrAve2;
		delete [] errProgr;
	}
	
	// Distruggo l'array dei walker.
	for(int i=0; i<quanti_walker; i++){
		f_out2<<wlk[i].getX()<<"   "<<wlk[i].getY()<<"   "<<wlk[i].getZ()<<endl;
	}
	delete [] wlk;

	
	f_out.close();
	f_out2.close();
	
	return 0;
}

double calcolaErrore(float * arrMedia, float * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
		cout<<"ArrMedia(i)="<<arrMedia[i]<<"    ArrMedia2[i]="<<arrMedia2[i]<<endl;
		cout<<"Passo "<<i<<", trovato errore "<<sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i)<<endl<<endl;
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

double funzione(double x){
	return M_PI/2.*cos(M_PI*x/2.)/(-2.*x+2.);
}

int scegliDirezione(Random * rnd){
	double x=rnd->Rannyu(0,3);
//	cout<<int(x)+1<<endl;
	return int(x)+1;
}

bool scegliVerso(Random * rnd){
	double x=rnd->Rannyu();
	if(x<0.5){
		return true;
	} else {
		return false;
	}
}

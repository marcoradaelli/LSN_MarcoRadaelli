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
	double volatility=0.25;
	
	// Questi array tengono i valori medi ottenuti in un blocco.
	// Numero totale di lanci.
	int M=1000000;
	int N=100;
	int L=M/N; // Ricordarsi di inserire M multiplo di N;
	// Creo gli array per salvare i dati.
	float * arrAve = new float[N];
	float * arrAve2 = new float[N];
	
	// -------------------------------
	
	// Eseguo i cicli.
	for (int i=0; i<N; i++){
		double somma = 0;
		for (int j=0; j<L; j++){
			// Qui la logica della valutazione. Alla fine, il valore va messo in somma.
			// Lancio una simulazione, cioe' calcolo il prezzo finale.
			double final_price;
			int steps_number=100;
			// Accumulo il prezzo step dopo step. Ad ogni step calcolo il successivo.
			// Inizialmente il prezzo precedente e' quello iniziale, poi lo cambio.
			double previous_price = initial_price;
			double Z_gaussian;
			for(int s=0; s<steps_number; s++){
				Z_gaussian=rnd.Gauss(0,1);
				previous_price=previous_price * exp((risk_free_rate-0.5*volatility*volatility)*delivery_time/(double)steps_number + volatility*Z_gaussian*sqrt(delivery_time/(double)steps_number));
			}
			// Il prezzo finale e' quello tirato fuori alla fine del ciclo.
			final_price=previous_price;
//			cout<<endl<<"Prezzo finale: "<<final_price<<endl;
			// Prendo il prezzo finale, gli tolgo lo strike price.
			// Per ora e' una put.
			if(-(final_price - strike_price)>0){
				// Salvo il valore, portandolo opportunamente indietro nel tempo.
				somma += -exp(-risk_free_rate * delivery_time) * (final_price - strike_price);
//				cout<<"Sfrutto la put"<<endl;
			} else {
				somma +=0;
//				cout<<"Non sfrutto la put"<<endl;
			}
		}
		// Alla fine di un blocco calcolo la media e la salvo nella casella dell'array.
		arrAve[i]=somma/(float)L;
		// E il quadrato della media.
		arrAve2[i]=arrAve[i]*arrAve[i];
		cout<<"Finito blocco "<<i<<endl;
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
//		cout<<"ProgrAve2["<<i<<"]="<<progrAve2[i]<<endl;
		errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
	}

	// Salvo su file per ogni blocco la media e la deviazione standard.
	ofstream fout1;
			
	fout1.open("profits.dat");

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
//		cout<<"ArrMedia(i)="<<arrMedia[i]<<"    ArrMedia2[i]="<<arrMedia2[i]<<endl;
//		cout<<"Passo "<<i<<", trovato errore "<<sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i)<<endl<<endl;
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

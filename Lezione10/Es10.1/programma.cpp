#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>

#include "citta.h"
#include "percorso.h"
#include "random.h"
#include "selezione.h"
#include "crossover.h"

using namespace std;

bool accetta(percorso vecchio, percorso proposto, double beta, citta * arr_citta, Random * rnd);

int main(int argc, char ** argv){
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
	
	// Qui il generatore di numeri casuali è avviato.
	
	// Variabili iniziali.
	int quante_citta = 30; // Quante sono le città da considerare.
	double raggio_circonferenza = 1.;
	int quanti_scambi_init = 30;
	double beta_min = 0.12;
	double beta_max = 100;
	double beta_step = 1;
	int quanti_tentativi_per_beta_step = 10000;
	
	ifstream file_in;
	file_in.open("input.dat");
	char str_away[30];
	file_in >> str_away >> quante_citta;
	file_in >> str_away >> quanti_scambi_init;
	file_in >> str_away >> beta_min; 
	file_in >> str_away >> beta_max;
	file_in >> str_away >> beta_step;
	file_in >> str_away >> quanti_tentativi_per_beta_step;
	
	file_in.close();
	
	int quante_beta = (beta_max - beta_min)/beta_step;
	
	// Costruisco l'array delle città.
	citta * arr_citta = new citta[quante_citta];
	
	// Stream out per il salvataggio delle città.
	ofstream out_citta;
	out_citta.open("posizioni_citta.dat");
	
	// Stream out per beta e lunghezze.
	ofstream out_lunghezze;
	out_lunghezze.open("migliori.dat");
	
	// Creo le città casualmente su una circonferenza.
	for(int i=0; i<quante_citta; i++){
		double theta = rnd.Rannyu(0,2.*M_PI);
		arr_citta[i].x = raggio_circonferenza * cos(theta);
		arr_citta[i].y = raggio_circonferenza * sin(theta);
		out_citta<<arr_citta[i].x<<"   "<<arr_citta[i].y<<endl;
	}
	
	// A questo punto sono pronte le città.
	
	// Creo un percorso. Inizialmente e' ordinato.
	percorso perc(quante_citta);
	// Eseguo un po' di scambi casuali.
	for(int i=0; i<quanti_scambi_init; i++){
		int a = int(rnd.Rannyu(0,quante_citta));
		int b = int(rnd.Rannyu(0,quante_citta));
		perc.scambia(a,b);
	}
	
	// Stampo il vettore da cui parto.
	cout<<"Stampo il vettore di partenza"<<endl;
	perc.stampa();
	
	// Ne verifico la consistenza.
	if(perc.controlla_consistenza()){
		cout<<"Controllata consistenza"<<endl;
	} else {
		cerr<<"Programma fallito: persa consistenza con il bound."<<endl;
		return 1;
	}
	
	//  Questo ciclo cicla sulle temperature.
	for(int i = 0; i < quante_beta; i++){
		// Calcolo il valore utilizzato della temperatura.
		double beta = beta_min + i * beta_step;
		int accettati_scambio = 0;
		int accettati_shift_contigue = 0;
		int accettati_permuta_blocchi = 0;
		
		// Questo ciclo cicla, a temperatura fissata, sulle mosse Metropolis.
		for(int j = 0; j < quanti_tentativi_per_beta_step; j++){
			percorso nuovo(quante_citta);
			// Inizialmente copio il vecchio percorso nel nuovo.
			nuovo = perc;
			// Eseguo una mutazione di scambio sul nuovo.
			int a = int(rnd.Rannyu(0,quante_citta));
			int b = int(rnd.Rannyu(0,quante_citta));
			nuovo.scambia(a,b);
			// Valuto se la mutazione e' accettabile dal Metropolis. Se si', la accetto.
			if(accetta(perc,nuovo,beta,arr_citta,&rnd)){
				perc = nuovo;
				accettati_scambio ++; 
			}
			
			// Copio ulteriormente il vecchio nel nuovo.
			nuovo = perc;
			// Eseguo una mutazione di shift delle contigue.
			int c = int(rnd.Rannyu(6,quante_citta-6));
			int d = int(rnd.Rannyu(1,5));
			nuovo.shift_contigue(quante_citta,c,d);
			// Valuto se la mutazione e' accettabile dal Metropolis. Se si', la accetto.
			if(accetta(perc,nuovo,beta,arr_citta,&rnd)){
				perc = nuovo;
				accettati_shift_contigue ++; 
			}
			
			// Copio ulteriormente il vecchio nel nuovo.
			nuovo = perc;
			// Eseguo una mutazione di permutazione a blocchi.
			int e = int(rnd.Rannyu(1,quante_citta-1));
			nuovo.permuta_blocchi(e);
			// Valuto se la mutazione e' accettabile dal Metropolis. Se si', la accetto.
			if(accetta(perc,nuovo,beta,arr_citta,&rnd)){
				perc = nuovo;
				accettati_permuta_blocchi ++;
			}
		}
		
//		cout << "Ok a beta = "<<beta<<"."<<endl;
		cout << "Beta = "<<beta<<" ; Lunghezza attuale = "<<perc.calcola_lunghezza(arr_citta)<<" ; Rate scambio = "<<(double)accettati_scambio/(double)quanti_tentativi_per_beta_step;
		cout << " ; Rate shift contigue = "<<(double)accettati_shift_contigue/(double)quanti_tentativi_per_beta_step;
		cout<<" ; Rate permuta blocchi = "<<(double)accettati_permuta_blocchi/(double)quanti_tentativi_per_beta_step<<endl;
//		perc.stampa();
		out_lunghezze << beta << "   " << perc.calcola_lunghezza(arr_citta) << endl;
		
		// Stampa un frame.
		ofstream out_frame;
		stringstream ss;
		ss << "./frames/frame_"<<i;
		string stringa = ss.str();
		out_frame.open(stringa);
		for(int i = 0; i < quante_citta ; i++){
			out_frame << arr_citta[perc.get_citta(i)].x << "   " << arr_citta[perc.get_citta(i)].y << endl;
		}
		out_frame << arr_citta[perc.get_citta(0)].x << "   " << arr_citta[perc.get_citta(0)].y << endl;
		out_frame.close();
	}
	
	// Stream out per lo scatterplot sui movimenti.
	ofstream out_movimenti;
	out_movimenti.open("movimenti.dat");
	for(int i = 0; i < quante_citta ; i++){
		out_movimenti << arr_citta[perc.get_citta(i)].x << "   " << arr_citta[perc.get_citta(i)].y << endl;
	}
	out_movimenti << arr_citta[perc.get_citta(0)].x << "   " << arr_citta[perc.get_citta(0)].y << endl;
	
	out_citta.close();
	out_movimenti.close();
	out_lunghezze.close();
	
	delete [] arr_citta;
}

bool accetta(percorso vecchio, percorso proposto, double beta, citta * arr_citta, Random * rnd){
	// Questa funzione verifica se, una volta proposta una mossa, essa vada accettata.
	// Se vado verso uno piu' corto accetto sempre.
	if(proposto.calcola_lunghezza(arr_citta) < vecchio.calcola_lunghezza(arr_citta)){
		return true;
	}
	
	// Altrimenti valuta le energie associate ai due percorsi.
	double diff_energia = - vecchio.calcola_lunghezza(arr_citta) + proposto.calcola_lunghezza(arr_citta);
	double limite_accetto = exp(- beta * diff_energia);
	double cas = rnd -> Rannyu();
	
	if(cas < limite_accetto){
		return true;
	} else {
		return false;
	}
}

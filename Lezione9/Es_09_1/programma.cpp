#include <iostream>
#include <cmath>
#include <fstream>

#include "citta.h"
#include "percorso.h"
#include "random.h"
#include "selezione.h"

using namespace std;

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
	
	// Qui il generatore di numeri casuali è avviato.
	
	// Variabili iniziali.
	int quanti_popolazione = 900; // Da quanti cromosomi è formata la popolazione. 
	int quante_citta = 30; // Quante sono le città da considerare.
	int quanti_scambi_creazione = 30;  // Quanti scambi casuali devo fare all'inizio.
	int quante_generazioni = 100; // Quante generazioni totali devo considerare.
	double raggio_circonferenza = 1.;
	
	// Costruisco l'array delle città.
	citta * arr_citta = new citta[quante_citta];
	
	// Qui creo la popolazione.
	percorso * arr_percorsi = new percorso[quanti_popolazione];
	percorso * arr_nuova_generazione; 
	// Inizializzo uno per uno i percorsi. Poi eseguo una serie di scambi per ogni percorso.
	for(int i=0; i<quanti_popolazione; i++){
		arr_percorsi[i].inizializza(quante_citta); // Inizializzo dichiarando il vettore.
		// Mescola casualmente più volte il vettore.
		for(int j=0; j<quanti_scambi_creazione; j++){
			int a = rnd.Rannyu(0,quante_citta);
			int b = rnd.Rannyu(0,quante_citta);
			arr_percorsi[i].scambia(a,b);
		}
	}
	
	// A questo punto ho la popolazione pronta.
	
	// Stream out per il salvataggio delle città.
	ofstream out_citta;
	out_citta.open("posizioni_citta.dat");
	
	// Creo le città casualmente su una circonferenza.
	for(int i=0; i<quante_citta; i++){
		double theta = rnd.Rannyu(0,2*M_PI);
		arr_citta[i].x = raggio_circonferenza * cos(theta);
		arr_citta[i].y = raggio_circonferenza * sin(theta);
		out_citta<<arr_citta[i].x<<"   "<<arr_citta[i].y<<endl;
	}
	
	// A questo punto sono pronte anche le città.
	
	// Stream out per il salvataggio dei valori medi delle distanze per generazione.
	ofstream out_media_generazione;
	out_media_generazione.open("media_generazione.dat");
	// Stream out per il salvataggio dei migliori per generazione.
	ofstream out_migliori;
	out_migliori.open("migliori.dat");
	
	// Questo ciclo cicla sulle generazioni.
	for(int generazione = 0; generazione<quante_generazioni; generazione++){
//		cout<<"Avviata generazione "<<generazione<<endl;
		// Alloco l'array per la nuova generazione.
		arr_nuova_generazione = new percorso[quanti_popolazione];
		for(int i=0; i<quanti_popolazione; i++){
			arr_nuova_generazione[i].inizializza(quante_citta);
		}
		// Questo ciclo cicla sugli elementi della nuova generazione. Cioè, alla fine di questo ciclo si deve essere costituita la nuova popolazione.
		for(int elemento = 0; elemento < quanti_popolazione; elemento++){
			// Seleziono uno degli elementi della vecchia generazione.
			int indice_selezionato = seleziona_percorso(arr_percorsi, quanti_popolazione, arr_citta, &rnd);
			// Copio l'elemento selezionato.
			percorso selezionato(arr_percorsi[indice_selezionato]);
			//cout<<"Avviata mutazione"<<endl;
			selezionato.muta(0.5, 0.2, 0.1, 0.05, 0.05, &rnd);
			if(selezionato.controlla_consistenza() == false){
				cerr<<"===Persa consistenza sulla mutazione in "<<elemento<<"alla generazione "<<generazione<<endl;
				selezionato.stampa();
				cout<<"==="<<endl;
			}
			//cout<<"Fine mutazione"<<endl;
//			cout<<"Fatto elemento "<<elemento<<" della generazione "<<generazione<<endl;
			// Inserisco il selezionato nella casella elemento dell'array della nuova generazione.
			arr_nuova_generazione[elemento] = selezionato;
		}
		
		// Introduco elitismo: il migliore della vecchia generazione si salva sempre.
		arr_nuova_generazione[0] = arr_percorsi[0];
		
		// A questo punto ho una intera nuova generazione. Elimino quella vecchia.
		delete [] arr_percorsi;
		arr_percorsi = arr_nuova_generazione;
		ordina_percorsi(arr_percorsi,quanti_popolazione,arr_citta);
		// Stampo il percorso migliore della generazione.
		arr_percorsi[0].stampa();
		
		// Calcolo una media delle distanze sulla nuova generazione.
		double accu = 0;
		for(int i=0; i<quanti_popolazione; i++){
			accu += arr_percorsi[i].calcola_lunghezza(arr_citta);
		}
		out_media_generazione<<generazione<<"   "<<accu/(double)quanti_popolazione<<endl;
		out_migliori<<generazione<<"   "<<arr_percorsi[0].calcola_lunghezza(arr_citta)<<endl;
		cout<<"Terminata generazione "<<generazione<<", lunghezza media = "<<accu/(double)quanti_popolazione<<"."<<endl;
	}	
	
	// Chiudo i file di output.
	out_media_generazione.close();
	out_citta.close();
	
	// Distruggo l'array delle città e dei percorsi.
	delete [] arr_citta;
	delete [] arr_percorsi;
}

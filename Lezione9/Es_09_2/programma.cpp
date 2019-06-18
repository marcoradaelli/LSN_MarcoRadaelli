#include <iostream>
#include <cmath>
#include <fstream>

#include "citta.h"
#include "percorso.h"
#include "random.h"
#include "selezione.h"
#include "crossover.h"

using namespace std;

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
	int quanti_popolazione = 300; // Da quanti cromosomi è formata la popolazione. 
	int quante_citta = 30; // Quante sono le città da considerare.
	int quanti_scambi_creazione = 30;  // Quanti scambi casuali devo fare all'inizio.
	int quante_generazioni = 100; // Quante generazioni totali devo considerare.
	double raggio_circonferenza = 1.;
	
	if(argc == 2){
		quante_generazioni = atoi(argv[1]);
	}
	
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
//		double theta = 2.*M_PI/(double)quante_citta * (double)i;
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
	// Stream out per lo scatterplot sui movimenti.
	ofstream out_movimenti;
	out_movimenti.open("movimenti.dat");
	
	// Questo ciclo cicla sulle generazioni.
	for(int generazione = 0; generazione<quante_generazioni; generazione++){
		cout<<"Avviata generazione "<<generazione<<endl;
		// Alloco l'array per la nuova generazione.
		arr_nuova_generazione = new percorso[quanti_popolazione];
		for(int i=0; i<quanti_popolazione; i++){
			arr_nuova_generazione[i].inizializza(quante_citta);
		}
		
		// Questo ciclo cicla sugli elementi della nuova generazione. Cioè, alla fine di questo ciclo si deve essere costituita la nuova popolazione.
		for(int elemento = 0; elemento < quanti_popolazione; elemento = elemento + 2){
			// Seleziono una coppia degli elementi della vecchia generazione.
			int indice_selezionato_1 = seleziona_percorso(arr_percorsi, quanti_popolazione, arr_citta, &rnd);
			int indice_selezionato_2 = seleziona_percorso(arr_percorsi, quanti_popolazione, arr_citta, &rnd);
//			cout<<"Selezionati indici "<<indice_selezionato_1<<" e "<<indice_selezionato_2<<"."<<endl;
			// Copio l'elemento selezionato.
			percorso selezionato_1(arr_percorsi[indice_selezionato_1]);
			percorso selezionato_2(arr_percorsi[indice_selezionato_2]);
//			cout<<"Copiati array correttamente."<<endl;
			//cout<<"Avviata mutazione"<<endl;
			
//			cout<<"-----------------------------------------------------------------------------------"<<endl;
//			cout<<"Stampo gli iniziali:"<<endl;
//			selezionato_1.stampa();
//			selezionato_2.stampa();
			
			// Eseguo un crossover tra i due selezionati.
			crossover(&selezionato_1, &selezionato_2, 5);
//			cout<<"Crossover eseguito; stampo i finali."<<endl;
//			selezionato_1.stampa();
//			selezionato_2.stampa();
			
			// Tento di mutare selezionato_1 e selezionato_2.
			selezionato_1.muta(0.5, 0.1, 0.1, 0.1, 0, &rnd);
			selezionato_2.muta(0.5, 0.1, 0.1, 0.1, 0, &rnd);
//			cout<<"-----------------------------------------------------------------------------------"<<endl;
						
			// Verifico che i due percorsi siano rimasti all'interno delle condizioni di consistenza.
			if(selezionato_1.controlla_consistenza() == false){
				cerr<<"===Persa consistenza sulla mutazione in "<<elemento<<"alla generazione "<<generazione<<endl;
				selezionato_1.stampa();
				cout<<"==="<<endl;
			}
			if(selezionato_2.controlla_consistenza() == false){
				cerr<<"===Persa consistenza sulla mutazione in "<<elemento<<"alla generazione "<<generazione<<endl;
				selezionato_2.stampa();
				cout<<"==="<<endl;
			}
			
			//cout<<"Fine mutazione"<<endl;
//			cout<<"Fatto elemento "<<elemento<<" della generazione "<<generazione<<endl;
			// Inserisco il selezionato nella casella elemento dell'array della nuova generazione.
			arr_nuova_generazione[elemento] = selezionato_1;
			arr_nuova_generazione[elemento+1] = selezionato_2;
		}
		
		// Introduco elitismo: il migliore della vecchia generazione si salva sempre.
//		arr_nuova_generazione[0] = arr_percorsi[0];
		
		// A questo punto ho una intera nuova generazione. Elimino quella vecchia.
		delete [] arr_percorsi;
		arr_percorsi = arr_nuova_generazione;
//		cout<<"CHIAMO ORDINAMENTO DAL MAIN"<<endl;
		ordina_percorsi(arr_percorsi,quanti_popolazione,arr_citta);
//		cout<<"CHIAMATO ORDINAMENTO DAL MAIN"<<endl;
		// Stampo il percorso migliore della generazione.
		cout<<"Percorso migliore della generazione: "<<endl;
		arr_percorsi[0].stampa();
//		cout<<"Fino a qui ok."<<endl;
		
		// Calcolo una media delle distanze sulla nuova generazione.
		double accu = 0;
		for(int i=0; i<quanti_popolazione/2; i++){
			accu += arr_percorsi[i].calcola_lunghezza(arr_citta);
		}
		out_media_generazione<<generazione<<"   "<<accu * 2./(double)quanti_popolazione<<endl;
		out_migliori<<generazione<<"   "<<arr_percorsi[0].calcola_lunghezza(arr_citta)<<endl;
		cout<<"Terminata generazione "<<generazione<<", lunghezza media = "<<accu * 2./(double)quanti_popolazione<<" ; lunghezza migliore = "<<arr_percorsi[0].calcola_lunghezza(arr_citta)<<endl;
	}
	
	percorso migliore_percorso = arr_percorsi[0];
	
	// Salvo il percorso migliore in un plot.
	for(int i=0; i<quante_citta; i++){
		out_movimenti<<arr_citta[migliore_percorso.get_citta(i)].x<<"   "<<arr_citta[migliore_percorso.get_citta(i)].y<<endl;
	}	
	// Chiudo il cammino.
	out_movimenti<<arr_citta[migliore_percorso.get_citta(0)].x<<"   "<<arr_citta[migliore_percorso.get_citta(0)].y<<endl;
	
	// Chiudo i file di output.
	out_media_generazione.close();
	out_citta.close();
	out_movimenti.close();
	
	// Distruggo l'array delle città e dei percorsi.
	delete [] arr_citta;
	delete [] arr_percorsi;
}

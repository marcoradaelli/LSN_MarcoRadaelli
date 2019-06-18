#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>

#include "citta.h"
#include "percorso.h"
#include "random.h"
#include "selezione.h"
#include "crossover.h"

#include "mpi.h"

using namespace std;

bool accetta(percorso vecchio, percorso proposto, double beta, citta * arr_citta, Random * rnd);
int trova_minimo(double * arr, int quanti);

int main(int argc, char ** argv){
	MPI::Init(argc, argv);
	
	int rank = MPI::COMM_WORLD.Get_rank();
	int size = MPI::COMM_WORLD.Get_size();
	
	ofstream out_lunghezze;
	if(rank == 0){
		out_lunghezze.open("lunghezze.dat");
	}
	
	// Avvio il generatore di numeri casuali.
	Random rnd;
	int seed[4];
	int p1, p2;
	ifstream Primes("Primes");
	if (Primes.is_open()){
	  // Comincio a scorrere il file di Primes in modo che ciascun processo prenda primes diversi.
	  for(int i = 0; i < rank ; i++){
	  	Primes >> p1 >> p2;
	  }
	  // Salvo solo alla fine. In questo modo ogni processo ha primes diversi.
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
	
	// Qui il generatore di numeri casuali è avviato (in parallelo).
	
	// Variabili iniziali.
	int quante_citta = 30; // Quante sono le città da considerare.
	double raggio_circonferenza = 1.;
	int quanti_scambi_init = 30;
	double beta_min = 0.12;
	double beta_max = 100;
	double beta_step = 1;
	int quanti_tentativi_per_beta_step = 10000;
	
	cout << "Ciao! Sono il nodo " << rank << " dei " << size << " che hai attivato." << endl;
	
	ifstream file_in;
	// Il file di input deve essere aperto da un processo alla volta.
	for(int i = 0; i<size ; i++){
		if(i == rank){
			file_in.open("input.dat");
			char str_away[30];
			file_in >> str_away >> quante_citta;
			file_in >> str_away >> quanti_scambi_init;
			file_in >> str_away >> beta_min; 
			file_in >> str_away >> beta_max;
			file_in >> str_away >> beta_step;
			file_in >> str_away >> quanti_tentativi_per_beta_step;
			file_in.close();
		}
	}
	
	// A questo punto e' pronto l'input parallelizzato.
	
	int quante_beta = (beta_max - beta_min)/beta_step;
	
	// Costruisco l'array delle città.
	citta * arr_citta = new citta[quante_citta];
	
	// Stream out per il salvataggio delle città.
	ofstream out_citta;
	if(rank == 0){  // Solo il processo master salva le città su file.
		out_citta.open("posizioni_citta.dat");
	}
	
	// Creo le città casualmente su una circonferenza. Ma lo fa solo il processo master.
	if(rank == 0){
		for(int i=0; i<quante_citta; i++){
			arr_citta[i].x = rnd.Rannyu(-1,1);
			arr_citta[i].y = rnd.Rannyu(-1,1);
			out_citta<<arr_citta[i].x<<"   "<<arr_citta[i].y<<endl;
		}
	}
	
	// Quindi a questo punto il processo master ha tutte le citta' salvate, e le deve trasferire agli altri.
	// Le citta' vanno trasferite una per una, e prima x e poi y.
	for(int i = 0; i < quante_citta; i++){
		MPI_Bcast(&(arr_citta[i].x),1,MPI_DOUBLE_PRECISION,0,MPI::COMM_WORLD);
		MPI_Bcast(&(arr_citta[i].y),1,MPI_DOUBLE_PRECISION,0,MPI::COMM_WORLD);
	}
	
	// A questo punto sono pronte le città copiate in tutti i processi.
	
	// Creo un percorso. Inizialmente e' ordinato. I percorsi sono diversi in tutti i processi.
	percorso perc(quante_citta);
	// Eseguo un po' di scambi casuali.
	for(int i=0; i<quanti_scambi_init; i++){
		int a = int(rnd.Rannyu(0,quante_citta));
		int b = int(rnd.Rannyu(0,quante_citta));
		perc.scambia(a,b);
	}
	
	// Stampo il vettore da cui parto.
//	cout<<"Stampo il vettore di partenza"<<endl;
//	perc.stampa();
	
	// Ne verifico la consistenza.
	if(perc.controlla_consistenza()){
//		cout<<"Controllata consistenza"<<endl;
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
			// Eseguo una mutazione di shift delle contigue.
			int e = int(rnd.Rannyu(1,quante_citta-1));
			nuovo.permuta_blocchi(e);
			// Valuto se la mutazione e' accettabile dal Metropolis. Se si', la accetto.
			if(accetta(perc,nuovo,beta,arr_citta,&rnd)){
				perc = nuovo;
				accettati_permuta_blocchi ++;
			}
		}
		// Stampo i risultati solo per il processo master.
		if(rank == 0){		
			cout << "Beta = "<<beta<<" ; Lunghezza attuale = "<<perc.calcola_lunghezza(arr_citta)<<" ; Rate scambio = "<<(double)accettati_scambio/(double)quanti_tentativi_per_beta_step;
			cout << " ; Rate shift contigue = "<<(double)accettati_shift_contigue/(double)quanti_tentativi_per_beta_step;
			cout<<" ; Rate permuta blocchi = "<<(double)accettati_permuta_blocchi/(double)quanti_tentativi_per_beta_step<<endl;
			out_lunghezze << beta << "   " << perc.calcola_lunghezza(arr_citta) << endl;
		}
	}
	
	// Qui salvo la lunghezza su questo core.
	double best_lunghezza = perc.calcola_lunghezza(arr_citta);
	
	// Se sei quello a rank 0, prendi tutti i risultati degli altri e salvali in un array.
	double * proposte = new double [size];
	
	// Tento di chiedere ai processi di farsi aspettare.
	MPI_Barrier(MPI::COMM_WORLD);
	
	// Importo tutte le lunghezze.
	MPI_Gather(&best_lunghezza,1,MPI_DOUBLE_PRECISION,proposte,1,MPI_DOUBLE_PRECISION,0,MPI::COMM_WORLD);
	// Trovo la lunghezza migliore.
	int processo_migliore = trova_minimo(proposte, size);  // Ma solo il master sa per ora qual è il processo migliore.
	// Trasmetto a tutti il processo migliore trovato dal master.
	MPI_Bcast(&(processo_migliore),1,MPI_INTEGER,0,MPI::COMM_WORLD);
	
	if(rank == 3){
		cout << "Ciao, sono 3" << endl;
	}
	
	// Qui il processo 3 esiste ancora.
	
	// Solo il processo 0 stampa.
	if(rank == 0){
		for(int i=0; i<size; i++){
			cout<<"Il processo "<<i<<" suggerisce una lunghezza "<<proposte[i]<<"."<<endl;
		}
		
		cout << "Il processo migliore è il " << processo_migliore << "." << endl;
	}
	
	if(rank == 3){
		cout << "Sono sempre il processo 3." << endl;
	}
	
	// Qui ho ancora il processo 3.
	
	// Trasferisco dal processo migliore i dati al processo master.
	percorso best(quante_citta);
	int i_esima_citta;
	for(int i = 0; i < quante_citta; i++){
		if(rank == processo_migliore){  // Il processo migliore trasmette la sua i-esima città.
			i_esima_citta = perc.get_citta(i);
			MPI::COMM_WORLD.Send(&i_esima_citta, 1, MPI::INTEGER, 0, 1);
//			cout << "Io, processo " << rank << " ho trasmesso al processo master." << endl;

		} else if(rank == 0){
			MPI::COMM_WORLD.Recv(&i_esima_citta, 1, MPI::INTEGER, processo_migliore, 1); 
			best.set_citta(i,i_esima_citta);
//			cout << "Io, processo master, ho ricevuto." << endl;
		}
	}
	
	ofstream out_movimenti;
	// Alla fine il processo master stampa su file i movimenti.
	if(rank == 0){
		cout << "Io, processo master, stampo i movimenti che mi ha mandato " << processo_migliore << endl;

		
		out_movimenti.open("movimenti.dat");
		for(int i = 0; i < quante_citta; i++){
			out_movimenti << arr_citta[best.get_citta(i)].x << "   " << arr_citta[best.get_citta(i)].y << endl;
			cout << arr_citta[best.get_citta(i)].x << "   " << arr_citta[best.get_citta(i)].y << endl;
		}
		out_movimenti << arr_citta[best.get_citta(0)].x << "   " << arr_citta[best.get_citta(0)].y << endl;
		out_movimenti.close();
	}
	
	if(rank == 0){
		out_lunghezze.close();
	}
	
	delete [] arr_citta;
	
	MPI::Finalize();
	
	return 0;
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

int trova_minimo(double * arr, int quanti){
	int loser = 0;
	for(int i = 0; i<quanti; i++){
		if(arr[i] < arr[loser]){
			loser = i;
		}
	}
	
	return loser;
}

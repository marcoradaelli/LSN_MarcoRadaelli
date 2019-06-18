#ifndef _PERCORSO_H_
#define _PERCORSO_H_

#include "citta.h"  // Mi serve includere le città perché ho bisogno di poter calcolare la lunghezza di un percorso.
#include "random.h"

class percorso{
	// Questa classe rappresenta un percorso.
	public:
		percorso(); // Non inizializza l'array, che va poi avviato manualmente con inizializza(quanti).
		percorso(int quanti); // Inizializza l'array. Poi costruisce la sequenza ordinata da 0 a quante_citta-1.
		percorso(const percorso&);   // Copy constructor.
		percorso& operator= (const percorso &nuovo);  // Overload operatore di assegnazione.
		void inizializza(int quanti); // Ha lo stesso esatto effetto di percorso(quanti), ma non in costruttore.
		double calcola_lunghezza(citta * arr_citta); // restituisce la lunghezza del percorso.
		bool controlla_consistenza(); // restituisce true se il percorso è consistente con i vincoli.
		void scambia(int i, int j); // scambia gli elementi i,j della sequenza.
		void stampa(); // stampa a schermo la sequenza di città considerata.
		void set_citta(int quale_posizione, int valore); // imposta una città precisa. Serve per debug.
		int get_citta(int quale_posizione);
		int get_quante_citta();
		
		// Qui tutte le possibili mutazioni che possono intervenire su un percorso.
		void shift(unsigned int n); 
		void shift_contigue(unsigned int numero_citta, unsigned int a_partire_da, unsigned int quante_posizioni);
		void permuta_blocchi(unsigned int numero_citta);
		void inverti_blocco(unsigned int da, unsigned int fino_a);
		
		// Questa procedura esegue tutte le possibili mutazioni con probabilità fissate dalla chiamante.
		void muta(double prob_scambia, double prob_shift, double prob_shift_contigue, double prob_permuta_blocchi, double prob_inverti_blocco, Random * rnd);
	
	private:
		int quante_citta;
		int * path;
		int pbc(int n); // Questa funzione gestisce il ritorno all'inizio dell'array.
};

// Questa funzione restituisce la distanza tra le due città.
double distanza(citta,citta);

// Questa funzione ordina il vettore.
int * ordina(int * array, int n);

#endif

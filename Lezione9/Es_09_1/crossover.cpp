#include "percorso.h"
#include "crossover.h"

void crossover(percorso * genitore_1, percorso * genitore_2, unsigned int dove_taglio){
	// Modifica i due genitori forniti eseguendo un crossover che non perda la consistenza con le condizioni del problema.
	// Creo due percorsi di appoggio con i vecchi percorsi.
	percorso old_1 = * genitore_1;
	percorso old_2 = * genitore_2;
	
	// Fino a dove_taglio, mi limito ad incrociare i percorsi.
	for(int i=0; i<dove_taglio; i++){
		genitore_1 -> set_citta(i,old_2.get_citta(i));
		genitore_2 -> set_citta(i,old_1.get_citta(i));
	}
	
	// A partire dal taglio, devo inserire i vecchi valori di old_1 in genitore_1
	// e di old_2 in genitore_2 ma rispettando la consistenza.
	
	// In questa variabile salvo il punto di partenza.
	int inserisci_qui = dove_taglio;
	
	// Ciclo sui valori di old_1.
	for(int i=0; i<old_1.get_quante_citta(); i++){
		int citta_da_provare = old_1.get_citta(i);
		bool flag_trovato = false; // Si alza questo flag se viene trovato già salvato.
		// Questa città è già presente in genitore_1 nella prima parte del crossover? Scorro genitore_1 per verificare.
		for(int j=0; j<dove_taglio; j++){
			if(genitore_1 -> get_citta(j) == citta_da_provare){
				flag_trovato = true;
			}
		}
		
		// A questo punto se ho trovato allora non faccio niente. Se non ho trovato inserisco e aumento la variabile di inserimento.
		if(!flag_trovato){
			genitore_1 -> set_citta(inserisci_qui, citta_da_provare);
			// La prossima volta devo inserire nella posizione successiva.
			inserisci_qui++;
		}
	}
	
	// Esattamente la stessa cosa su old_2 e genitore_2.
	for(int i=0; i<old_2.get_quante_citta(); i++){
		int citta_da_provare = old_2.get_citta(i);
		bool flag_trovato = false; // Si alza questo flag se viene trovato già salvato.
		// Questa città è già presente in genitore_2 nella prima parte del crossover? Scorro genitore_2 per verificare.
		for(int j=0; j<dove_taglio; j++){
			if(genitore_2 -> get_citta(j) == citta_da_provare){
				flag_trovato = true;
			}
		}
		
		// A questo punto se ho trovato allora non faccio niente. Se non ho trovato inserisco e aumento la variabile di inserimento.
		if(!flag_trovato){
			genitore_2 -> set_citta(inserisci_qui, citta_da_provare);
			// La prossima volta devo inserire nella posizione successiva.
			inserisci_qui++;
		}
	}
}

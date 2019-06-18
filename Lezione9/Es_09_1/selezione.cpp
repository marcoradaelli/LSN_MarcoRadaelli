#include <iostream>
#include <cmath>

#include "citta.h"
#include "percorso.h"
#include "random.h"

using namespace std;

void ordina_percorsi(percorso * arr_percorsi, int quanti, citta * arr_citta){
	// Questa procedura mette i percorsi in ordine di lunghezza.
	
	int min;
	percorso temp;
	for(int i=0; i<quanti-1; i++){
		min = i;
		for(int j=i+1; j<quanti; j++){
		 	if(arr_percorsi[j].calcola_lunghezza(arr_citta) < arr_percorsi[min].calcola_lunghezza(arr_citta)){ 
		    		min = j;
		    	}
		}
		temp=arr_percorsi[min];
		arr_percorsi[min]=arr_percorsi[i];
		arr_percorsi[i]=temp;
	}
}

int seleziona_percorso(percorso * arr_percorsi, int quanti, citta* arr_citta, Random * rnd){
	// Prima di tutto ordina il vettore dei percorsi.
	//ordina_percorsi(arr_percorsi, quanti, arr_citta);
	
	// A questo punto devo decidere quale percorso selezionare.
	// Scelgo un numero intero sulla base di un algoritmo semplificato (vedi jupyter).
	double potenza = 3.;
	int scelto = int(quanti * pow(rnd->Rannyu(),potenza));
	
	return scelto;
}

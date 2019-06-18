#include <iostream>
#include <cmath>

#include "citta.h"
#include "percorso.h"
#include "random.h"

using namespace std;

/*void ordina_percorsi(percorso * arr_percorsi, int quanti, citta * arr_citta){
	// Questa procedura mette i percorsi in ordine di lunghezza.
	cout<<"Sono l'ordinamento chiamato dal main."<<endl;
	
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
		cout<<"Confrontati fino a "<<i<<endl;
	}
	
	cout<<"Fine ordinamento chiamato dal main."<<endl;
}*/

int seleziona_percorso(percorso * arr_percorsi, int quanti, citta* arr_citta, Random * rnd){
	// Prima di tutto ordina il vettore dei percorsi.
	//ordina_percorsi(arr_percorsi, quanti, arr_citta);
	
	// A questo punto devo decidere quale percorso selezionare.
	// Scelgo un numero intero sulla base di un algoritmo semplificato (vedi jupyter).
	double potenza = 3.;
	int scelto = int(quanti * pow(rnd->Rannyu(),potenza));
	
	return scelto;
}

void ordina_percorsi(percorso * arr_percorsi, int quanti, citta * arr_citta){
	// Misuro tutte le lunghezze e le salvo in un array.
	double * arr_lunghezze = new double[quanti];
	int * arr_indici = new int[quanti];
	
	for(int i=0; i<quanti; i++){
		arr_lunghezze[i] = arr_percorsi[i].calcola_lunghezza(arr_citta);
		arr_indici[i] = i;
	}
	
	// A questo punto non ricalcolo più le lunghezze, ma ordino l'array di double ricordandomi di cambiare gli indici.
	int min;
	double temp_double;
	int temp_int;
	for(int i=0; i<quanti-1; i++){
		min = i;
		for(int j=i+1; j<quanti; j++){
		 	if(arr_lunghezze[j] < arr_lunghezze[min]){ 
		    		min = j;
		    	}
		}
		temp_double = arr_lunghezze[min];
		temp_int = arr_indici[min];
		arr_lunghezze[min]=arr_lunghezze[i];
		arr_indici[min] = arr_indici[i];
		arr_lunghezze[i]=temp_double;
		arr_indici[i]=temp_int;
//		cout<<"Confrontati fino a "<<i<<endl;
	}
	
	// Alla fine creo un nuovo array.
	percorso * nuovo_arr_percorsi = new percorso[quanti];
	for(int i=0; i<quanti; i++){
		nuovo_arr_percorsi[i] = arr_percorsi[arr_indici[i]]; 
	}
	
	for(int i=0; i<quanti; i++){
		arr_percorsi[i] = nuovo_arr_percorsi[i];
	}
	
	delete [] nuovo_arr_percorsi;
}

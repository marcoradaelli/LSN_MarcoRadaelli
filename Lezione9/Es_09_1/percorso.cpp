#include "citta.h"
#include "percorso.h"
#include <iostream>
#include <cmath>

using namespace std;

percorso::percorso(int quanti){
	// Alloca l'array di quanti elementi.
	quante_citta = quanti;
	path = new int[quanti];
	
	// Carica nell'array le città nell'ordine.
	for(int i=0; i<quanti; i++){
		path[i] = i;
	}
	
	// Alla fine di questo costruttore l'array contiene tutte le città nell'ordine.
}

percorso::percorso(){
	quante_citta = 0;
	// Quando si usa questo costruttore, dopo bisogna inizializzare a mano l'array.
}	

void percorso::inizializza(int quanti){
	// Alloca l'array di quanti elementi.
	quante_citta = quanti;
	path = new int[quanti];
	
	// Carica nell'array le città nell'ordine.
	for(int i=0; i<quanti; i++){
		path[i] = i;
	}
	
	// Alla fine di questa procedura l'array contiene tutte le città nell'ordine.
}

double percorso::calcola_lunghezza(citta * arr_citta){
	// Questa funzione ritorna la lunghezza del percorso.
	double accu = 0;
	for(int i=0; i<quante_citta - 1; i++){
		accu += distanza(arr_citta[path[i]], arr_citta[path[i+1]]);
	}
	// Alla fine sommo la distanza tra l'ultima città e la città 0.
	accu += distanza(arr_citta[path[0]],arr_citta[path[quante_citta-1]]);
	
	return accu;
}

void percorso::scambia(int i, int j){
	int appo;
	appo = path[pbc(i)];
	path[pbc(i)] = path[pbc(j)];
	path[pbc(j)] = appo;
}

double distanza(citta c1,citta c2){
	return pow((c1.x - c2.x)*(c1.x - c2.x) + (c1.y - c2.y)*(c1.y - c2.y) , 0.5);
}

void percorso::stampa(){
	cout<<endl<<"Stampo vettore delle città"<<endl;
	for(int i=0; i<quante_citta; i++){
		cout<<path[i]<<"   ";
	}
	cout<<endl;
}

bool percorso::controlla_consistenza(){
	int * ordinato = ordina(path,quante_citta);
	for(int i=0; i<quante_citta; i++){
		if(i != ordinato[i]){
			delete [] ordinato;
			return false;
		}
	}
	
	delete [] ordinato;
	
	return true;
}

int * ordina(int * array, int n){
	// Restituisce un array ordinato dei dati del primo array.
	
	int * vet = new int [n];
	// Fa una copia dei dati dell'array.
	for(int i=0; i<n; i++){
		vet[i] = array[i];
	}
	
	int min,temp;
	for(int i=0; i<n-1; i++){
		min = i;
		for(int j=i+1; j<n; j++){
		 	if(vet[j] < vet[min]){ //cambiare questa condizione per invertire l'ordine
		    		min = j;
		    	}
		}
		temp=vet[min];
		vet[min]=vet[i];
		vet[i]=temp;
	}
	
	return vet;
}

void percorso::set_citta(int quale_posizione, int valore){
	path[quale_posizione] = valore;
}

void percorso::shift(unsigned int n){
	// Questa procedura shifta i valori.
	for(int i=0; i<n; i++){
		// Questo ciclo interno shifta i valori di 1.
		int appo = path[0];
		for(int j=0; j<quante_citta-1; j++){
			scambia(j,j+1);
		}
	}
}

int percorso::pbc(int i){
	if(i >= quante_citta) i = i - quante_citta;
	else if(i < 0) i = i + quante_citta;
	return i;
}

void percorso::shift_contigue(unsigned int numero_citta, unsigned int a_partire_da, unsigned int quante_posizioni){
	// Questa procedura sposta avanti di quante_posizioni numero_citta a partire da quella in posizione a_partire_da.
	// Questo ciclo esterno cicla sulle città da spostare.
	for(int i = a_partire_da+numero_citta; i>a_partire_da; i--){
		// La città da spostare all'inizio si trova in posizione path[i]. Adesso la scambio successivamente per quante_posizioni volte con la successiva.
		for(int j=i;j<i+quante_posizioni; j++){
			scambia(j,j+1);
		}
		
//		cout<<"Spostata la città in posizione iniziale "<<i<<" di "<<quante_posizioni<<" caselle."<<endl;
//		stampa();
	}
}

void percorso::permuta_blocchi(unsigned int numero_citta){
	// Questa procedura scambia le prime numero_citta città con le ultime numero_citta città.
	for(int i=0; i<numero_citta; i++){
		scambia(i,quante_citta - numero_citta + i);
	}
}

void percorso::inverti_blocco(unsigned int da, unsigned int fino_a){
	// Questa procedura inverte l'ordine delle città nel blocco da da a fino_a.
	if((fino_a - da) % 2 != 0){   // se il numero di città da scambiare è dispari:
		for(int i=da; i<(fino_a-da)/2+1; i++){
			scambia(i, fino_a - i);
		}
	} else {  // se invece è pari:
		for(int i=da; i<(fino_a-da)/2; i++){
			scambia(i, fino_a - i);
		}
	}
}

void percorso::muta(double prob_scambia, double prob_shift, double prob_shift_contigue, double prob_permuta_blocchi, double prob_inverti_blocco, Random * rnd){
	// Questa procedura esegue tutte le possibili mutazioni, con probabilità definita dalla chiamante.
	// Scambio.
	if(rnd->Rannyu() < prob_scambia){
		int i = rnd->Rannyu(0,quante_citta);
		int j = rnd->Rannyu(0,quante_citta);
		scambia(i,j);
	}
	
	// Shift.
	if(rnd->Rannyu() < prob_shift){
		int n = rnd->Rannyu(0,quante_citta);
		shift(1);
	}
	
	// Shift delle contigue.
	if(rnd->Rannyu() < prob_shift_contigue){
		int numero_citta = rnd->Rannyu(0,quante_citta-3);
		int a_partire_da = rnd->Rannyu(0,quante_citta-8);
		int quante_posizioni = rnd->Rannyu(0,3);
		shift_contigue(2,0,1);
	}
	
	// Permutazione a blocchi.
	if(rnd->Rannyu() < prob_permuta_blocchi){
		int n = rnd->Rannyu(0,quante_citta/2-1);
		permuta_blocchi(n);
	}
	
	if(rnd->Rannyu() < prob_inverti_blocco){
		int i = rnd->Rannyu(0,quante_citta-4);
		int j = rnd->Rannyu(j,quante_citta);
		inverti_blocco(i,j);
	}
}

percorso::percorso(const percorso& nuovo){
	quante_citta = nuovo.quante_citta;
	path = new int[quante_citta];
	for(int i=0; i<quante_citta; i++){
		path[i] = nuovo.path[i];
	}
}

percorso& percorso::operator= (const percorso &nuovo)
{
	quante_citta = nuovo.quante_citta;
	path = new int[quante_citta];
	for(int i=0; i<quante_citta; i++){
		path[i] = nuovo.path[i];
	}
 
    // return the existing object so we can chain this operator
    return *this;
}

int percorso::get_citta(int posizione){
	return path[posizione];
}

int percorso::get_quante_citta(){
	return quante_citta;
}

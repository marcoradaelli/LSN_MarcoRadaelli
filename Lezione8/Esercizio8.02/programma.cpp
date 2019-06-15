#include "random.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

struct punto{
	// Questa struct definisce un punto nello spazio. Downgrade a 1D dei conti usati per l'atomo di idrogeno.
	double x;
};

double pdf(punto p, double mu, double sigma);
bool accetta(punto init, punto fin, double mu, double sigma, Random * rnd);
punto proponiPunto(punto init, double delta, Random * rnd);
double integranda(punto p, double mu, double sigma);
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
	} else cerr << "PROBLEM: Unable to open seed.in" << endl;
	
	// Qui ho inizializzato il generatore di numeri casuali.
	
	// Qui impostazione della delta.
	double delta =2.5;
	// Qui impostazione di mu e sigma.
	double mu = 0.805389;
	double sigma = 0.610831;
	
	// Posizione iniziale.
	punto puntoAttuale;
	puntoAttuale.x = 0;
	
	double min_x = -5.;
	double max_x = 5.;
	
	int bin_x = 100;
	double dim_bin = (max_x - min_x)/(double)bin_x;
	
	int * istogramma = new int[bin_x];
	
	int quanti_punti = 1000000;
	
	// Imposto a zero tutti i campi dell'istogramma.
	for(int i=0; i<bin_x; i++){
		istogramma[i] = 0;
	}
	
	int n_accettati=0;
	
	double accu=0; // Questo accumulatore mi serve per normalizzare.
	
	// Eseguo i cicli.
	for(int i=0; i<quanti_punti; i++){
		punto tentativo = proponiPunto(puntoAttuale,delta, &rnd);
		// Accetto il tentativo?
		if(accetta(puntoAttuale,tentativo,mu,sigma,&rnd)){
			puntoAttuale = tentativo;
			n_accettati++;
		}
		// Inserisco il risultato in un istogramma.
		if(puntoAttuale.x > min_x and puntoAttuale.x < max_x){
			int n = int((puntoAttuale.x - min_x)/dim_bin);
			istogramma[n]++;
		}
	}
	
	cout<<"Rapporto di accettazione: "<<(double)n_accettati / (double)quanti_punti<<endl;
	
	// Scrive l'istogramma su file.
	ofstream file_out;
	file_out.open("istogramma.dat");
	for(int i=0; i<bin_x; i++){
		file_out<<min_x+dim_bin*i<<"   "<<istogramma[i] / dim_bin / (double)quanti_punti<<endl;
//		cout<<min_x+dim_bin*i<<"   "<<istogramma[i]<<endl;
	}
	
	file_out.close();
	
	cout<<"Esecuzione conclusa"<<endl;
	
	delete [] istogramma;
	
	return 0;
}

double pdf(punto p, double mu, double sigma){
	// Questa funzione restituisce il valore della distribuzione di probabilita' in un certo punto.
	double x;
	x=p.x;
	
	double exp1 = exp(-pow(x-mu,2.)/pow(sigma,2.));
	double exp2 = exp(-pow(x+mu,2.)/pow(sigma,2.));
	double exp3 = exp((-pow(x,2.) - pow(mu,2.)) / pow(sigma,2.));
	
	return exp1 + exp2 + 2.*exp3; // Qui la funzione.
}

bool accetta(punto init, punto fin, double mu, double sigma, Random * rnd){
	// Decide se accettare o meno un certo lancio.
	if(pdf(fin,mu,sigma)>pdf(init,mu,sigma)){
		return true;
	} else {
		double limiteAccetto=pdf(fin, mu, sigma)/pdf(init, mu, sigma);
		// Lancia uniformemente tra 0 e 1.
		double ran = rnd->Rannyu();
		if (ran<limiteAccetto){
			return true;
		} else {
			return false;
		}
	}
}

punto proponiPunto(punto init, double delta, Random * rnd){
	// Questa procedura fa fare un passo. Qui il meccanismo di transizione.
	// Estraggo casualmente.
	punto fin;
	fin.x=rnd->Rannyu(init.x-delta, init.x+delta);
	
	return fin;
}

double integranda(punto p, double mu, double sigma){
	// Questa funzione restituisce il valore dell'integranda nel punto p desiderato.
	double x = p.x;;
	
	double expN = exp(- pow(x-mu, 2.) / 2. / pow(sigma,2.));
	double expP = exp(- pow(x+mu, 2.) / 2. / pow(sigma,2.));
	double parN = pow(x-mu,2.) / pow(sigma,4.) - pow(sigma, -2.);
	double parP = pow(x+mu,2.) / pow(sigma,4.) - pow(sigma, -2.);
	
	double psi = expN + expP;
	double potenziale = pow(x,4.) - 2.5 * pow(x,2.);
	
	return -0.5 * (expN*parN + expP*parP)/psi + potenziale;
}

double calcolaErrore(float * arrMedia, float * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}



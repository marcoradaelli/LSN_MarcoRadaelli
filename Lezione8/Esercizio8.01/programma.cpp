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
	
	int M=1000000;
	int N=100;
	int L=M/N; // Ricordarsi di inserire M multiplo di N;
	// Creo gli array per salvare i dati.
	float * arrAve = new float[N];
	float * arrAve2 = new float[N];
	
	ofstream out_posizioni;
	out_posizioni.open("positions.dat");
	
	// Eseguo i cicli.
	for (int i=0; i<N; i++){
		double somma = 0;
		double numero;
		int n_accettati = 0;
		for (int j=0; j<L; j++){
			// Ogni passaggio consiste in un tentativo di movimento e poi nella valutazione della funzione.
			// Tento di spostarmi.
			punto tentativo = proponiPunto(puntoAttuale,delta, &rnd);
			// Accetto il tentativo?
			if(accetta(puntoAttuale,tentativo,mu,sigma,&rnd)){
				puntoAttuale = tentativo;
				n_accettati++;
			}
			
			// Valuto la funzione r nel punto.
			somma+= integranda(puntoAttuale,mu,sigma);   // Qui la funzione valutata nel punto.
			
			// Salvo il punto in cui mi trovo su file.
			out_posizioni<<puntoAttuale.x<<endl;
		}
		// Alla fine di un blocco calcolo la media e la salvo nella casella dell'array.
		arrAve[i]=somma/(float)L;
		// E il quadrato della media.
		arrAve2[i]=arrAve[i]*arrAve[i];
		// E scrivo il rapporto di accettazione.
		cout<<"Blocco "<<i<<": rate di accettazione "<<(double)n_accettati / (double)L<<endl;
	}
	
	// A questo punto ho nei due array le medie di tutti i blocchi e le medie quadrate di tutti i blocchi.
	float * progrAve = new float[N];
	float * progrAve2 = new float[N];
	float * errProgr = new float[N];
	// Adesso ciclo sugli array che ho creato accumulando.
	for (int i=0; i<N; i++){
		progrAve[i]=0;
		progrAve2[i]=0;
		for (int j=0; j<i+1; j++){
			progrAve[i]+=arrAve[j];
			progrAve2[i]+=arrAve2[j];
		}
		
		progrAve[i]/=(float)(i+1);
		progrAve2[i]/=(float)(i+1);
//		cout<<"ProgrAve2["<<i<<"]="<<progrAve2[i]<<endl;
		errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
	}

	// Salvo su file per ogni blocco la media e la deviazione standard.
	ofstream fout1;
			
	fout1.open("data.dat");

	for(int i=0; i<N; i++){
		fout1<<i<<"   "<<progrAve[i]<<"   "<<errProgr[i]<<endl;
	}

	delete [] arrAve;
	delete [] arrAve2;
	delete [] progrAve;
	delete [] progrAve2;
	delete [] errProgr;
	
	fout1.close();
	out_posizioni.close();
	
	cout<<"Esecuzione conclusa"<<endl;
	
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



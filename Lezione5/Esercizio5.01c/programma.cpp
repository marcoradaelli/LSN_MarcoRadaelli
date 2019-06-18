#include "random.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

struct punto{
	// Questa struct definisce un punto nello spazio.
	double x,y,z;
};

double pdf(punto p);
bool accetta(punto init, punto fin, Random * rnd);
punto proponiPunto(punto init, double delta, Random * rnd);
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
	
	// --------------------------------------------------
	
	// In questa prima fase cerco di capire se va bene il valore di delta che ho scelto.
	// Eseguo 100 passi.
	// Punto iniziale.
	punto puntoAttuale;
	puntoAttuale.x = 1;
	puntoAttuale.y = 1;
	puntoAttuale.z = 1;
	
	int counter_accettati=0;
	
	double delta =1;
	
	for(int i=0; i<100; i++){
		punto tentativo = proponiPunto(puntoAttuale, delta, &rnd);
		// Accetto il tentativo?
		if(accetta(puntoAttuale,tentativo,&rnd)){
			puntoAttuale = tentativo;
			counter_accettati++;
		}
	}
	
	cout<<"Ho accettato "<<counter_accettati<<" su 100 punti, cioe' il "<<(double)counter_accettati<<" %"<<endl<<endl;
	
	// --------------------------------------------------
	
	// Questo codice ha il compito di capire qual e' il delta giusto.
	int tolleranza = 5;
	while(abs(counter_accettati-50)>tolleranza){
		counter_accettati=0;
		// Reimposta il delta. Aumenta se counter_accettati e' troppo alto, scende se e' basso.
		if(counter_accettati>50){
			delta = delta + 0.05;
		} else {
			delta = delta - 0.05;
		}
		
		// Qui ciclo.
		for(int i=0; i<100; i++){
		punto tentativo = proponiPunto(puntoAttuale, delta, &rnd);
		// Accetto il tentativo?
			if(accetta(puntoAttuale,tentativo,&rnd)){
				puntoAttuale = tentativo;
				counter_accettati++;
			}
		}	
		cout<<"Ho accettato "<<counter_accettati<<" su 100 punti, cioe' il "<<(double)counter_accettati<<" %, usando delta = "<<delta<<endl<<endl;	
	}
	
	// --------------------------------------------------
	// A questo punto delta dovrebbe essere impostata.
	
	int M=10000;
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
		for (int j=0; j<L; j++){
			// Ogni passaggio consiste in un tentativo di movimento e poi nella valutazione della funzione.
			// Tento di spostarmi.
			punto tentativo = proponiPunto(puntoAttuale,delta, &rnd);
			// Accetto il tentativo?
			if(accetta(puntoAttuale,tentativo,&rnd)){
				puntoAttuale = tentativo;
			}
			
			// Valuto la funzione r nel punto.
			somma+= sqrt(puntoAttuale.x*puntoAttuale.x + puntoAttuale.y*puntoAttuale.y + puntoAttuale.z*puntoAttuale.z);
			
			// Salvo il punto in cui mi trovo su file.
			out_posizioni<<puntoAttuale.x<<"   "<<puntoAttuale.y<<"   "<<puntoAttuale.z<<endl;
		}
		// Alla fine di un blocco calcolo la media e la salvo nella casella dell'array.
		arrAve[i]=somma/(float)L;
		// E il quadrato della media.
		arrAve2[i]=arrAve[i]*arrAve[i];
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

double pdf(punto p){
	// Questa funzione restituisce il valore della funzione in un certo punto.
	double x,y,z;
	x=p.x;
	y=p.y;
	z=p.z;
	double r=sqrt(x*x + y*y + z*z);
	return 1./M_PI * exp(-2*r); // Qui la funzione.
}

bool accetta(punto init, punto fin, Random * rnd){
	// Decide se accettare o meno un certo lancio.
	if(pdf(fin)>pdf(init)){
		return true;
	} else {
		double limiteAccetto=pdf(fin)/pdf(init);
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
	// Estraggo casualmente, ma stavolta in maniera gaussiana.
	punto fin;
	fin.x=rnd->Gauss(init.x, delta);
	fin.y=rnd->Gauss(init.y, delta);
	fin.z=rnd->Gauss(init.z, delta);
	
	return fin;
}

double calcolaErrore(float * arrMedia, float * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

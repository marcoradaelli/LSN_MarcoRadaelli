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
int trovaMinimo(double * array, int dimensione);

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
	double delta =0.9;
	
	double mu_min = 0.79;
	double mu_max = 0.83;
	double sigma_min = 0.5;
	double sigma_max = 0.7;
	int quanti_sigma = 10;
	int quanti_mu = 10;
	int step_equil = 10000;
	bool ricerca_auto = false;
	int step_auto = 5;
	
	// Carico da file i dati di input.
	ifstream file_in;
	file_in.open("input.dat");
	
	char* string_away = new char[60];
	int appo;  // Questa variabile serve per salvare il risultato di ricerca_auto fino a che non viene convertito in bool.
	
	file_in >> string_away >> delta;
	file_in >> string_away >> mu_min;
	file_in >> string_away >> mu_max;
	file_in >> string_away >> sigma_min;
	file_in >> string_away >> sigma_max;
	file_in >> string_away >> quanti_mu;
	file_in >> string_away >> quanti_sigma;
	file_in >> string_away >> step_equil;
	file_in >> string_away >> appo;
	file_in >> string_away >> step_auto;
	file_in.close();
	
	// Converto appo nella variabile bool ricerca_auto.
	if(appo == 1){
		ricerca_auto = true;
	} else {
		ricerca_auto = false;
		step_auto = 1;   // Esegue un solo step (cioè non cerca automaticamente il minimo).
	}
	
	
	double step_mu = (double)(mu_max - mu_min)/(double)quanti_mu;
	double step_sigma = (double)(sigma_max - sigma_min)/(double)quanti_sigma;
	
	ofstream out_data;
	out_data.open("ottimizzazione.dat");
	
	for(int tent_auto = 0; tent_auto < step_auto; tent_auto++){
		// In questi array salvo man mano i tentativi di mu, sigma e il risultato di energia ottenuto.
		double * arr_mu = new double[quanti_mu * quanti_sigma];
		double * arr_sigma = new double[quanti_mu * quanti_sigma];
		double * arr_energie = new double[quanti_mu * quanti_sigma];
		
		cout<<"Avviato tentativo "<<tent_auto+1<<"/"<<step_auto<<"   con mu_min "<<mu_min<<"   mu_max "<<mu_max<<"   sigma_min "<<sigma_min<<"   sigma_max "<<sigma_max<<endl;
		
		for(int count_mu = 0 ; count_mu < quanti_mu ; count_mu ++){
			for(int count_sigma = 0; count_sigma < quanti_sigma ; count_sigma ++){
				double mu = mu_min + count_mu * step_mu;
				double sigma = sigma_min +  count_sigma  * step_sigma;
				
				// A questo punto lancio la simulazione con questi valori di sigma e mu. 
				// Posizione iniziale.
				punto puntoAttuale;
				puntoAttuale.x = 0;
				
				// Step di equilibrazione.
				for(int eq=0; eq<step_equil; eq++){
					punto tentativo = proponiPunto(puntoAttuale,delta, &rnd);
					// Accetto il tentativo?
					if(accetta(puntoAttuale,tentativo,mu,sigma,&rnd)){
						puntoAttuale = tentativo;
					}
				}
				
				int M=1000000;
				int N=1000;
				int L=M/N; // Ricordarsi di inserire M multiplo di N;
				// Creo gli array per salvare i dati.
				float * arrAve = new float[N];
				float * arrAve2 = new float[N];
				
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
					}
					// Alla fine di un blocco calcolo la media e la salvo nella casella dell'array.
					arrAve[i]=somma/(float)L;
					// E il quadrato della media.
					arrAve2[i]=arrAve[i]*arrAve[i];
					// E scrivo il rapporto di accettazione.
	//				cout<<"Blocco "<<i<<": rate di accettazione "<<(double)n_accettati / (double)L<<endl;
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

				// Salvo su file i risultati di ciascun blocco.
				out_data<<mu<<"   "<<sigma<<"   "<<progrAve[N-1]<<"   "<<errProgr[N-1]<<endl;
				cout<<mu<<"   "<<sigma<<"   "<<progrAve[N-1]<<"    "<<errProgr[N-1]<<endl;
				
				// Salva anche negli array il risultato.
				arr_mu[count_mu * quanti_mu + count_sigma] = mu;
				arr_sigma[count_mu * quanti_mu + count_sigma] = sigma;
				arr_energie[count_mu * quanti_mu + count_sigma] = progrAve[N-1];

				delete [] arrAve;
				delete [] arrAve2;
				delete [] progrAve;
				delete [] progrAve2;
				delete [] errProgr;
			}
		}
		
		// Cerca il minimo nell'array delle energie.
		int indice_minimo = trovaMinimo(arr_energie, quanti_mu * quanti_sigma);
		double ottima_mu = arr_mu[indice_minimo];
		double ottima_sigma = arr_sigma[indice_minimo];
		
		cout<<"Trovati valori ottimali:      mu = "<<ottima_mu<<"       sigma = "<<ottima_sigma<<"   energia = "<<arr_energie[indice_minimo]<<endl<<endl<<endl;
		
		// Riduce le maglie della rete in modo da rieseguire le due celle centrali.
		step_mu = step_mu / quanti_mu * 2.;
		step_sigma = step_sigma / quanti_sigma * 2.;
		// Ricentra la rete sui valori ottimali in modo da essere metà sopra e metà sotto.
		mu_min = ottima_mu - 0.5 * quanti_mu * step_mu;
		mu_max = ottima_mu + 0.5 * quanti_mu * step_mu;
		sigma_min = ottima_sigma - 0.5 * quanti_sigma * step_sigma;
		sigma_max = ottima_sigma + 0.5 * quanti_sigma * step_sigma;
		
		// A questo punto è pronta per ripartire.
		
		delete [] arr_mu;
		delete [] arr_sigma;
		delete [] arr_energie;
	}
	
	out_data.close();
	
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

int trovaMinimo(double * array, int dimensione){
	int winner=0;
	for(int i=0; i<dimensione; i++){
		if(array[winner] > array[i]){
			winner = i;
		}
	}
	
	return winner;
}

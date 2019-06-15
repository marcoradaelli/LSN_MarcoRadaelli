#include <fstream>
#include <iostream>

using namespace std;

// Questa funzione riceve in ingresso l'array delle medie e stampa su file la media a blocchi con suo errore.

void mediaBlocchi(double * arrAve, int quantiBlocchi, ofstream * file_out);
double calcolaErrore(float * arrMedia, float * arrMedia2, int i);

void mediaBlocchi(double * arrAve, int quantiBlocchi, ofstream * file_out){
	// Creo l'array delle medie quadrate.
	double * arrAve2 = new double[quantiBlocchi];
	// E lo carico.
	for(int i=0; i<quantiBlocchi; i++){
		arrAve2[i] = arrAve[i] * arrAve[i];
	}
	
	// Creo i tre array di accumulo.
	double * progrAve = new double[quantiBlocchi];
	double * progrAve2 = new double[quantiBlocchi];
	double * errProgr = new double[quantiBlocchi];
	
	for (int i=0; i<quantiBlocchi; i++){
		progrAve[i]=0;
		progrAve2[i]=0;
		for (int j=0; j<i+1; j++){
			progrAve[i]+=arrAve[j];
			progrAve2[i]+=arrAve2[j];
		}
		
		progrAve[i]/=(float)(i+1);
		progrAve2[i]/=(float)(i+1);
		cout<<"ProgrAve2["<<i<<"]="<<progrAve2[i]<<endl;
		errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
	}
	
	// Qui il meccanismo di stampa.		
	for(int i=0; i<N; i++){
		(*file_out)<<i<<"   "<<progrAve[i]<<"   "<<errProgr[i]<<endl;
	}

	delete [] arrAve2;
	delete [] progrAve;
	delete [] progrAve2;
	delete [] errProgr;
}

double calcolaErrore(float * arrMedia, float * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
//		cout<<"ArrMedia(i)="<<arrMedia[i]<<"    ArrMedia2[i]="<<arrMedia2[i]<<endl;
//		cout<<"Passo "<<i<<", trovato errore "<<sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i)<<endl<<endl;
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

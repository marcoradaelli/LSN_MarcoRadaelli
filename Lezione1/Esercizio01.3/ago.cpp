#include <iostream>
#include <cmath>
#include "ago.h"
#include "random.h"

using namespace std;

ago::ago(double x, double y, double theta, double lung){
	x_cm=x;
	y_cm=y;
	_theta=theta;
	_l=lung;
}

double ago::getX(){
	return x_cm;
}

double ago::getY(){
	return y_cm;
}

double ago::getL(){
	return _l;
}

double ago::getTheta(){
	return _theta;
}

void ago::stampaAgo(){
	cout<<"Ago: x_cm="<<x_cm<<"   y_cm="<<y_cm<<"   L="<<_l<<"   theta="<<_theta<<endl;
}

bool ago::interseca(double distLin){
	// Calcola il punto più alto e quello più basso dell'ago.
	double y_max, y_min;
	y_max=y_cm+_l/2*cos(_theta);
	y_min=y_cm-_l/2*cos(_theta);
	
	// Verifica che siano correttamente ordinati, altrimenti scambia.
	if(y_max<y_min){
		double appo=y_max;
		y_max=y_min;
		y_min=appo;
	}
	// Alla fine di questo if sono sicuro che y_min e y_max siano nell'ordine corretto.
	
	// Verifica quante volte il passo reticolare è contenuto in y_max.
	int counter_max=0;
	counter_max=int(y_max/distLin);
	
	// Lo stesso con y_min.
	int counter_min=0;
	counter_min=int(y_min/distLin);
	
	// Se i due count sono diversi allora l'ago tocca la linea.
	if(counter_min<counter_max){
//		cout<<"L'ago tocca la linea"<<endl;
		return true;
	} else if(counter_min==counter_max){
//		cout<<"L'ago non tocca la linea"<<endl;
		return false;
	} else {
		cout<<"ERRORE: i dati non sono consistenti."<<endl;
		return false;
	}
	
	return false;
}

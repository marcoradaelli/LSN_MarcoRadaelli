#include <iostream>
#include <cmath>
#include "random.h"
#include "walker.h"

using namespace std;

walker_discreto::walker_discreto(){
	// Crea un walker discreto nella posizione di origine.
	_x=0;
	_y=0;
	_z=0;
}

walker_discreto::walker_discreto(int x, int y, int z){
	// Crea un walker discreto nella posizione di origine.
	_x=x;
	_y=y;
	_z=z;
}

void walker_discreto::passo(int direzione, bool avanti){
//	cout<<"Ho ricevuto la direzione "<<direzione<<endl;
	if(direzione==1){
		if(avanti){
			_x++;
			return;
		} else {
			_x--;
			return;
		}
	}
	
	if(direzione==2){
		if(avanti){
			_y++;
			return;
		} else {
			_y--;
			return;
		}
	}
	
	if(direzione==3){
		if(avanti){
			_z++;
			return;
		} else {
			_z--;
			return;
		}
	} 
	
	cerr<<"ERRORE: Non ho trovato la direzione in cui vuoi farmi procedere."<<endl;
}

void walker_discreto::stampa(){
	cout<<"Sono il walker in ("<<_x<<";"<<_y<<";"<<_z<<")"<<endl;
}

double walker_discreto::distanzaDaOrigine(double passoReticolare){
	double x=_x*passoReticolare;
	double y=_y*passoReticolare;
	double z=_z*passoReticolare;
	
	return sqrt(x*x+y*y+z*z);
}

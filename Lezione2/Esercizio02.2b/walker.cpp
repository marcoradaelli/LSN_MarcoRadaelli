#include <iostream>
#include <cmath>
#include "random.h"
#include "walker.h"

using namespace std;

walker_continuo::walker_continuo(){
	// Crea un walker discreto nella posizione di origine.
	_x=0;
	_y=0;
	_z=0;
}

walker_continuo::walker_continuo(double x, double y, double z){
	// Crea un walker discreto nella posizione di origine.
	_x=x;
	_y=y;
	_z=z;
}

void walker_continuo::passo(double theta, double phi){
//	cout<<"Ho ricevuto le istruzioni di avanzare di 1 verso "<<theta<<"   "<<phi<<endl;
	_x+=sin(theta)*cos(phi);
	_y+=sin(theta)*sin(phi);
	_z+=cos(theta);
//	stampa();
}

void walker_continuo::stampa(){
	cout<<"Sono il walker in ("<<_x<<";"<<_y<<";"<<_z<<")"<<endl;
}

double walker_continuo::distanzaDaOrigine(){
	double x=_x;
	double y=_y;
	double z=_z;
	
	return sqrt(x*x+y*y+z*z);
}

void walker_continuo::azzera(){
	_x=0;
	_y=0;
	_z=0;
}

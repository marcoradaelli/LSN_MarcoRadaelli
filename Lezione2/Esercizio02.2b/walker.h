#ifndef _WALKER_
#define _WALKER_

#include <iostream>
#include <cmath>
#include "random.h"

using namespace std;

class walker_continuo{
	private:
		double _x,_y,_z;
		
	public:
		walker_continuo(); // Crea un walker nella posizione (0,0,0).
		walker_continuo(double x, double y, double z); //  Crea un walker nella posizione desiderata.
		void passo(double theta, double phi); // Fa muovere il walker nella direzione desiderata (1=x,2=y,3=z) e nel vers desiderato.
		void azzera();  // Riporta alla posizione iniziale.
		void stampa(); // Stampa la posizione attuale del walker.
		double distanzaDaOrigine(); // Restituisce la distanza del walker dall'origine.
};

#endif

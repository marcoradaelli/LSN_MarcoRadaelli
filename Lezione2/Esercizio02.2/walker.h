#ifndef _WALKER_
#define _WALKER_

#include <iostream>
#include <cmath>
#include "random.h"

using namespace std;

class walker_discreto{
	private:
		int _x,_y,_z;
		
	public:
		walker_discreto(); // Crea un walker nella posizione (0,0,0).
		walker_discreto(int x, int y, int z); //  Crea un walker nella posizione desiderata.
		void passo(int direzione, bool avanti); // Fa muovere il walker nella direzione desiderata (1=x,2=y,3=z) e nel verso desiderato.
		void stampa(); // Stampa la posizione attuale del walker.
		double distanzaDaOrigine(double passoReticolare); // Restituisce la distanza del walker dall'origine.
};

#endif

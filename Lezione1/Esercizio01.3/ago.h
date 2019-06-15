#ifndef _AGO_H_
#define _AGO_H_

// Questa classe rappresenta l'oggetto ago, con la sua posizione e il suo orientamento. 
class ago{
	private:
		double x_cm,y_cm; // queste variabili contengono la posizione del Centro di Massa dell'ago.
		double _theta; // questa variabile contiene l'angolo rispetto alla verticale dell'ago.
		double _l; // questa variabile contiene la lunghezza dell'ago.
	
	public: 
		ago(double x, double y, double theta, double lunghezza); // Questo costruttore imposta già l'ago con i dati forniti.
		double getX();
		double getY();
		double getL();
		double getTheta();
		bool interseca(double distLin); // Questo metodo verifica se l'ago creato interseca con le righe di un reticolo distLin.
		void stampaAgo();
};

#endif

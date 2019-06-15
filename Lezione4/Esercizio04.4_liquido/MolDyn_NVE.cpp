/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
#include <stdlib.h>     // srand, rand: to generate random number
#include <iostream>     // cin, cout: Standard Input/Output Streams Library
#include <fstream>      // Stream class to both read and write from/to files.
#include <cmath>        // rint, pow
#include "MolDyn_NVE.h"

using namespace std;

void mediaBlocchi(double * arrAve, int quantiBlocchi, ofstream * file_out);
double calcolaErrore(double * arrMedia, double * arrMedia2, int i);
double distanza(int i, int j);

int main(){ 
  Input();             //Inizialization
  int nconf = 1;
  
  int quantiBlocchi = 100;
  int stepPerBlocco = nstep / quantiBlocchi; // Inserire un multiplo!
  
  double * arrAve_epot = new double[quantiBlocchi];
  double * arrAve_ekin = new double[quantiBlocchi];
  double * arrAve_etot = new double[quantiBlocchi];
  double * arrAve_t = new double[quantiBlocchi];
  double * arrAve_press = new double[quantiBlocchi];
  
  ofstream out_veloce;
  out_veloce.open("energies.out");
  
  // Questo ciclo cicla sui blocchi.
  for(int i=0; i<quantiBlocchi; i++){
  	// Azzera il valore in quel blocco dell'array.
  	arrAve_epot[i]=0;
  	arrAve_ekin[i]=0;
  	arrAve_etot[i]=0;
  	arrAve_t[i]=0;
  	arrAve_press[i]=0;
  	// Calcola la media su quel blocco continuando a muoversi.
  	for(int j=0; j<stepPerBlocco; j++){
  		// Eseguo la misura.
  		Measure();
  		// Registro la misura dell'energia potenziale (per ora).
  		arrAve_epot[i] += stima_pot;
  		arrAve_ekin[i] += stima_kin;
  		arrAve_etot[i] += stima_etot;
  		arrAve_t[i] += stima_temp;
  		arrAve_press[i] += stima_press;
  		
  		out_veloce<<i*stepPerBlocco + j<<"   "<<stima_etot<<"   "<<stima_kin<<"   "<<stima_pot<<endl;
  		
  		// Faccio una mossa Verlet.
  		Move();
  		
  		if((i*stepPerBlocco + j)%1000 == 0){
  			cout<<"Eseguito passo "<<j<<" del blocco "<<i<<", cioè "<<i*stepPerBlocco + j<<" sul totale"<<endl;
  		}
  	}
  	// A questo punto in arrAve[i] ho la somma delle energie potenziali per gli step. Normalizzo sul numero di step.
  	arrAve_epot[i] = arrAve_epot[i] / (double)stepPerBlocco;
  	arrAve_ekin[i] = arrAve_ekin[i] / (double)stepPerBlocco;
  	arrAve_etot[i] = arrAve_etot[i] / (double)stepPerBlocco;
  	arrAve_t[i] = arrAve_t[i] / (double)stepPerBlocco;
  	arrAve_press[i] = arrAve_press[i] / (double)stepPerBlocco;
  }
  
  cout<<"Terminata la simulazione. Calcolo le medie a blocchi."<<endl;
  
  // A questo punto in arrAve ho tutte le medie di tutti i blocchi. Lo passo a mediaBlocchi insieme ad un puntatore al meccanismo di stampa.
  ofstream out_epot;
  ofstream out_ekin;
  ofstream out_etot;
  ofstream out_t;
  ofstream out_press;
  
  out_epot.open("ave_epot.out");
  out_ekin.open("ave_ekin.out");
  out_etot.open("ave_etot.out");
  out_t.open("ave_t.out");
  out_press.open("ave_press.out");
  
  mediaBlocchi(arrAve_epot, quantiBlocchi, &out_epot);
  mediaBlocchi(arrAve_ekin, quantiBlocchi, &out_ekin);
  mediaBlocchi(arrAve_etot, quantiBlocchi, &out_etot);
  mediaBlocchi(arrAve_t, quantiBlocchi, &out_t);
  mediaBlocchi(arrAve_press, quantiBlocchi, &out_press);
  
  ConfFinal();         //Write final configuration to restart
  
  delete [] arrAve_epot;
  delete [] arrAve_ekin;
  delete [] arrAve_etot;
  delete [] arrAve_t;
  delete [] arrAve_press;
  
  out_epot.close();
  out_ekin.close();
  out_etot.close();
  out_t.close();
  out_press.close();

  return 0;
}


void Input(void){ //Prepare all stuff for the simulation
  ifstream ReadInput,ReadConf;
  double ep, ek, pr, et, vir;

  cout << "Classic Lennard-Jones fluid        " << endl;
  cout << "Molecular dynamics simulation in NVE ensemble  " << endl << endl;
  cout << "Interatomic potential v(r) = 4 * [(1/r)^12 - (1/r)^6]" << endl << endl;
  cout << "The program uses Lennard-Jones units " << endl;

  seed = 1;    //Set seed for random numbers
  srand(seed); //Initialize random number generator
  
  ReadInput.open("input.dat"); //Read input

  ReadInput >> temp;

  ReadInput >> npart;
  cout << "Number of particles = " << npart << endl;

  ReadInput >> rho;
  cout << "Density of particles = " << rho << endl;
  vol = (double)npart/rho;
  cout << "Volume of the simulation box = " << vol << endl;
  box = pow(vol,1.0/3.0);
  cout << "Edge of the simulation box = " << box << endl;

  ReadInput >> rcut;
  ReadInput >> delta;
  ReadInput >> nstep;
  ReadInput >> iprint;
  ReadInput >> loadOldData;

  cout << "The program integrates Newton equations with the Verlet method " << endl;
  cout << "Time step = " << delta << endl;
  cout << "Number of steps = " << nstep << endl << endl;
  ReadInput.close();

//Prepare array for measurements
  iv = 0; //Potential energy
  ik = 1; //Kinetic energy
  ie = 2; //Total energy
  it = 3; //Temperature
  n_props = 4; //Number of observables

//Read initial configuration
  cout << "Read initial configuration from file config.0 " << endl << endl;
  ReadConf.open("config.0");
  for (int i=0; i<npart; ++i){
    ReadConf >> x[i] >> y[i] >> z[i];
    x[i] = x[i] * box;
    y[i] = y[i] * box;
    z[i] = z[i] * box;
  }
  ReadConf.close();
  
// Read previous configuration if loadOldData == 1
  if (loadOldData == 1){
      cout << "Read previous configuration from file old.0.";
      ifstream ReadOld;
      ReadOld.open("old.0");
      for (int i=0; i<npart; ++i){
        ReadOld >> xold[i] >> yold[i] >> zold[i];
        xold[i] = xold[i] * box;
        yold[i] = yold[i] * box;
        zold[i] = zold[i] * box;
//        cout<<"Posizione precedente: x["<<i<<"](t-dt) = "<< xold[i]<<endl;
      }
      ReadOld.close();
      
      // A questo punto ho negli array xold, yold e zold e negli array x, y e z le posizioni. Faccio uno step di Verlet per calcolare le posizioni a tempo t+dt.
      // Ma prima devo salvare i due array vecchi, altrimenti Verlet sovrascrive.
      // Posizioni x0=x(t-dt), x1=x(t), x2=x(t+dt).
      double x0[m_part], y0[m_part], z0[m_part];
      double x1[m_part], y1[m_part], z1[m_part];
      
      // Copio gli array.
      for (int i=0; i<npart; i++){
      	x0[i] = xold[i];
      	y0[i] = yold[i];
      	z0[i] = zold[i];
      	x1[i] = x[i];
      	y1[i] = y[i];
      	z1[i] = z[i];
//      	cout<<"x["<<i<<"](t-dt) = "<<x0[i]<<"  ;  x["<<i<<"](t) = "<<x1[i]<<endl;
      }
      
      // Faccio uno step di Verlet.
      Move();
      // Adesso negli array x,y,z ho i valori correnti della posizione a tempo t+dt.
      // Posso ricavare la velocità.
      double sumv[3]={0.0, 0.0, 0.0};
      for(int i=0; i<npart; i++){
      	vx[i] = Pbc(x[i] - x0[i])/(2.*delta);
      	vy[i] = Pbc(y[i] - y0[i])/(2.*delta);
      	vz[i] = Pbc(z[i] - z0[i])/(2.*delta);
//      	cout<<"vx["<<i<<"]="<<vx[i]<<"   vy["<<i<<"]="<<vy[i]<<"   vz["<<i<<"]="<<vz[i]<<endl;
      	sumv[0] += vx[i];
      	sumv[1] += vy[i];
      	sumv[2] += vz[i];
      }
      
      // A questo punto quindi ho negli array vx,vy,vz la velocità delle particelle alla fine della simulazione precedente. Per il momento però le velocità non sono ancora riscalate
      // tenendo in considerazione la temperatura.
      // Da qui il sistema di riscalamento.
      double sumv2=0, fs;
      for(int i=0; i<npart; i++){
      	sumv2 += vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
      }
      
      sumv2 /= (double)npart;
      
      fs = sqrt(3*temp/sumv2);
     for (int i=0; i<npart; ++i){
     vx[i] *= fs;
     vy[i] *= fs;
     vz[i] *= fs;

     xold[i] = Pbc(x[i] - vx[i] * delta);
     yold[i] = Pbc(y[i] - vy[i] * delta);
     zold[i] = Pbc(z[i] - vz[i] * delta);
  }
  }
  
//Prepare initial velocities. Questo secondo metodo funziona solo se loadOldData == 0.
  if(loadOldData == 0){
	   cout << "Prepare random velocities with center of mass velocity equal to zero " << endl << endl;
	   double sumv[3] = {0.0, 0.0, 0.0};
	   for (int i=0; i<npart; ++i){
	     vx[i] = rand() - 0.5;
	     vy[i] = rand() - 0.5;
	     vz[i] = rand() - 0.5;

	     sumv[0] += vx[i];
	     sumv[1] += vy[i];
	     sumv[2] += vz[i];
	   }
	   for (int idim=0; idim<3; ++idim) sumv[idim] /= (double)npart;
	   double sumv2 = 0.0, fs;
	   for (int i=0; i<npart; ++i){
	     vx[i] = vx[i] - sumv[0];
	     vy[i] = vy[i] - sumv[1];
	     vz[i] = vz[i] - sumv[2];

	     sumv2 += vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
	   }
	   sumv2 /= (double)npart;

	   fs = sqrt(3 * temp / sumv2);   // fs = velocity scale factor 
	   for (int i=0; i<npart; ++i){
	     vx[i] *= fs;
	     vy[i] *= fs;
	     vz[i] *= fs;

	     xold[i] = Pbc(x[i] - vx[i] * delta);
	     yold[i] = Pbc(y[i] - vy[i] * delta);
	     zold[i] = Pbc(z[i] - vz[i] * delta);
	   }
   }
   return;
}


void Move(void){ //Move particles with Verlet algorithm
  double xnew, ynew, znew, fx[m_part], fy[m_part], fz[m_part];

  for(int i=0; i<npart; ++i){ //Force acting on particle i
    fx[i] = Force(i,0);
    fy[i] = Force(i,1);
    fz[i] = Force(i,2);
  }

  for(int i=0; i<npart; ++i){ //Verlet integration scheme

    xnew = Pbc( 2.0 * x[i] - xold[i] + fx[i] * pow(delta,2) );
    ynew = Pbc( 2.0 * y[i] - yold[i] + fy[i] * pow(delta,2) );
    znew = Pbc( 2.0 * z[i] - zold[i] + fz[i] * pow(delta,2) );

    vx[i] = Pbc(xnew - xold[i])/(2.0 * delta);
    vy[i] = Pbc(ynew - yold[i])/(2.0 * delta);
    vz[i] = Pbc(znew - zold[i])/(2.0 * delta);

    xold[i] = x[i];
    yold[i] = y[i];
    zold[i] = z[i];

    x[i] = xnew;
    y[i] = ynew;
    z[i] = znew;
  }
  return;
}

double Force(int ip, int idir){ //Compute forces as -Grad_ip V(r)
  double f=0.0;
  double dvec[3], dr;

  for (int i=0; i<npart; ++i){
    if(i != ip){
      dvec[0] = Pbc( x[ip] - x[i] );  // distance ip-i in pbc
      dvec[1] = Pbc( y[ip] - y[i] );
      dvec[2] = Pbc( z[ip] - z[i] );

      dr = dvec[0]*dvec[0] + dvec[1]*dvec[1] + dvec[2]*dvec[2];
      dr = sqrt(dr);

      if(dr < rcut){
        f += dvec[idir] * (48.0/pow(dr,14) - 24.0/pow(dr,8)); // -Grad_ip V(r)
      }
    }
  }
  
  return f;
}

void Measure(){ //Properties measurement
  int bin;
  double v, t, vij;
  double dx, dy, dz, dr;
  ofstream Epot, Ekin, Etot, Temp, Press;

  Epot.open("output_epot.dat",ios::app);
  Ekin.open("output_ekin.dat",ios::app);
  Temp.open("output_temp.dat",ios::app);
  Etot.open("output_etot.dat",ios::app);
  Press.open("output_press.dat",ios::app);

  v = 0.0; //reset observables
  t = 0.0;

//cycle over pairs of particles
  for (int i=0; i<npart-1; ++i){
    for (int j=i+1; j<npart; ++j){

     dx = Pbc( x[i] - x[j] );
     dy = Pbc( y[i] - y[j] );
     dz = Pbc( z[i] - z[j] );

     dr = dx*dx + dy*dy + dz*dz;
     dr = sqrt(dr);

     if(dr < rcut){
       vij = 4.0/pow(dr,12) - 4.0/pow(dr,6);

//Potential energy
       v += vij;
     }
    }          
  }

//Kinetic energy
  for (int i=0; i<npart; ++i) t += 0.5 * (vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i]);
  
//Pressione.
  double P=0;
  double accu=0;
  for(int i=0; i<m_part; i++){
  	for(int j=i+1; j<m_part; j++){
  		// La pressione viene calcolata a partire dal viriale.
  		accu += 48.*(pow(distanza(i,j),-12) - 0.5 * pow(distanza(i,j),-6));
  	}
  }
  
    P=rho*temp + 1./(3.*vol)*accu;
   
    stima_pot = v/(double)npart; //Potential energy
    stima_kin = t/(double)npart; //Kinetic energy
    stima_temp = (2.0 / 3.0) * t/(double)npart; //Temperature
    stima_etot = (t+v)/(double)npart; //Total enery
    stima_press = P; //Pressure

    Epot << stima_pot  << endl;
    Ekin << stima_kin  << endl;
    Temp << stima_temp << endl;
    Etot << stima_etot << endl;
    Press << stima_press << endl;

    Epot.close();
    Ekin.close();
    Temp.close();
    Etot.close();
    Press.close();

    return;
}


void ConfFinal(void){ //Write final configuration and the previous one.
  ofstream WriteConf;
  ofstream WriteOld;

  cout << "Print final configuration to file config.final " << endl << endl;
  WriteConf.open("config.final");
  cout << "Print previous configuration to file old.final " << endl << endl;
  WriteOld.open("old.final");

  for (int i=0; i<npart; ++i){
    WriteConf << x[i]/box << "   " <<  y[i]/box << "   " << z[i]/box << endl;
    WriteOld << xold[i]/box << "   " << yold[i]/box << "   " << zold[i]/box <<endl;
  }
  WriteConf.close();
  WriteOld.close();
  
  return;
}

void ConfXYZ(int nconf){ //Write configuration in .xyz format
  ofstream WriteXYZ;

  WriteXYZ.open("frames/config_" + to_string(nconf) + ".xyz");
  WriteXYZ << npart << endl;
  WriteXYZ << "This is only a comment!" << endl;
  for (int i=0; i<npart; ++i){
    WriteXYZ << "LJ  " << Pbc(x[i]) << "   " <<  Pbc(y[i]) << "   " << Pbc(z[i]) << endl;
  }
  WriteXYZ.close();
}

double Pbc(double r){  //Algorithm for periodic boundary conditions with side L=box
    return r - box * rint(r/box);
}


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
//		cout<<"ProgrAve2["<<i<<"]="<<progrAve2[i]<<endl;
		errProgr[i]=calcolaErrore(progrAve,progrAve2,i);
	}
	
	// Qui il meccanismo di stampa.		
	for(int i=0; i<quantiBlocchi; i++){
		(*file_out)<<i<<"   "<<progrAve[i]<<"   "<<errProgr[i]<<endl;
	}

	delete [] arrAve2;
	delete [] progrAve;
	delete [] progrAve2;
	delete [] errProgr;
}

double calcolaErrore(double * arrMedia, double * arrMedia2, int i){
	if (i==0){
		return 0;
	} else {
//		cout<<"ArrMedia(i)="<<arrMedia[i]<<"    ArrMedia2[i]="<<arrMedia2[i]<<endl;
//		cout<<"Passo "<<i<<", trovato errore "<<sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i)<<endl<<endl;
		return sqrt((arrMedia2[i]-arrMedia[i]*arrMedia[i])/i);
	}
}

double distanza(int i, int j){
	// Questa funzione restituisce la distanza attuale tra la particella di indice i e la particella di indice j.
	return sqrt( pow(x[i]-x[j],2) + pow(y[i]-y[j],2) + pow(z[i]-z[j],2) );
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

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

int main(){ 
  Input();             //Inizialization
  int nconf = 1;
  for(int istep=1; istep <= nstep; ++istep){
     Move();           //Move particles with Verlet algorithm
     if(istep%iprint == 0) cout << "Number of time-steps: " << istep << endl;
     if(istep%10 == 0){ // Misuro solo ogni 10 step
        Measure();     //Properties measurement
        ConfXYZ(nconf);//Write actual configuration in XYZ format //Commented to avoid "filesystem full"! 
        nconf += 1;
     }
  }
  ConfFinal();         //Write final configuration to restart

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
        cout<<"Posizione precedente: x["<<i<<"](t-dt) = "<< xold[i]<<endl;
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
      	cout<<"x["<<i<<"](t-dt) = "<<x0[i]<<"  ;  x["<<i<<"](t) = "<<x1[i]<<endl;
      }
      
      // Faccio uno step di Verlet.
      Move();
      // Adesso negli array x,y,z ho i valori correnti della posizione a tempo t+dt.
      // Posso ricavare la velocità.
      double sumv[3]={0.0, 0.0, 0.0};
      for(int i=0; i<npart; i++){
      	vx[i] = (Pbc(x[i] - x0[i]))/(2.*delta);
      	vy[i] = (Pbc(y[i] - y0[i]))/(2.*delta);
      	vz[i] = (Pbc(z[i] - z0[i]))/(2.*delta);
      	cout<<"vx["<<i<<"]="<<vx[i]<<"   vy["<<i<<"]="<<vy[i]<<"   vz["<<i<<"]="<<vz[i]<<endl;
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
      
      // Media quadratica delle velocità.
      sumv2 /= (double)npart;
      
      
      fs = sqrt(3*temp/sumv2);
      
     // Riscalo i valori di velocità. 
     for (int i=0; i<npart; ++i){
     vx[i] *= fs;
     vy[i] *= fs;
     vz[i] *= fs;

     // Qui ho inserito le Periodic Boundary Conditions.
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

	     // Anche qui ho inserito le Periodic Boundary Conditions.
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
  ofstream Epot, Ekin, Etot, Temp;

  Epot.open("output_epot.dat",ios::app);
  Ekin.open("output_ekin.dat",ios::app);
  Temp.open("output_temp.dat",ios::app);
  Etot.open("output_etot.dat",ios::app);

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
   
    stima_pot = v/(double)npart; //Potential energy
    stima_kin = t/(double)npart; //Kinetic energy
    stima_temp = (2.0 / 3.0) * t/(double)npart; //Temperature
    stima_etot = (t+v)/(double)npart; //Total enery

    Epot << stima_pot  << endl;
    Ekin << stima_kin  << endl;
    Temp << stima_temp << endl;
    Etot << stima_etot << endl;

    Epot.close();
    Ekin.close();
    Temp.close();
    Etot.close();

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
/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

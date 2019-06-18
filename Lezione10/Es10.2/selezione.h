#ifndef _SELEZIONE_H_
#define _SELEZIONE_H_

#include "citta.h"
#include "percorso.h"

// Procedure che eseguono la selezione.
void ordina_percorsi(percorso * arr_percorsi, int quanti, citta * arr_citta);  // Questa procedura mette i percorsi in ordine di lunghezza.
int seleziona_percorso(percorso * arr_percorsi, int quanti, citta* arr_citta, Random * rnd);  

#endif

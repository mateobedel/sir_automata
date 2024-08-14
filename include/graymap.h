#ifndef _GRAYMAP_H_
#define _GRAYMAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include "automata.h"


typedef struct liste_points_voisins liste_points_voisins;


//Structure d'une graymap de densité
struct PGMData {
    int nx;
    int ny;
    int max_gris;
    int **t;
}; typedef struct PGMData PGMData;


//Lit une image .pgm (graymap) puis renvoi la matrice de densité associé
void lirePGM(char* dir, PGMData* data);


//Free une graymap
void freePGM(PGMData* data);


#endif
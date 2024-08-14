#ifndef _CELLULAR_AUTOMATA_H_
#define _CELLULAR_AUTOMATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "graymap.h"
#include "utils.h"
#include "var.h"




typedef struct PGMData PGMData;


//Point voisin d'une cellule C
struct point_voisin {
    int x;
    int y;
    //Taux d'infection entre la cellule C et la cellule voisine :
    double tx_inf_voisin; 
}; typedef struct point_voisin point_voisin;


//Liste des points voisins d'une cellule
struct liste_points_voisins {
    point_voisin* liste_points;
    int nombre;
}; typedef struct liste_points_voisins liste_points_voisins;


//Cellule de l'automate cellulaire
struct cellule {
    double dens;
    double* sain; //proportion de sain
    double* inf; //proportion d'infectés
    double* ret; //proportion de rétablis
}; typedef struct cellule cellule;


//Génère une liste de points voisins dans un disque de propagation donné autour des coordonnées (xc, yc)
// - tx_inf: paramètre d'infection du système
liste_points_voisins points_disque_propagation(int rayon, int xc, int yc, cellule** automate, float tx_inf);


//Initialise la grille de cellules dans l'automate cellulaire avec des valeurs initiales.
// - pgm_dens: pointeur vers une structure PGMData contenant une graymap de densité (peut être NULL)
// - f: fonction de densité (si il n'y a pas de matrice de densité)
cellule** aut_init(int nombre_jours, double (*f)(double, double), PGMData* pgm_dens);


//Free l'automate
void aut_free(cellule** automate);


//Calcule la matrice des listes des cellules voisines pour chaque cellule dans l'automate.
// - rayon_propagation: rayon de propagation des cellules voisines
// - tx_inf: facteur d'infection
liste_points_voisins** cellules_voisines(cellule** automate, float tx_inf, int rayon_propagation);


//Free les celulles voisines
void free_cellules_voisines(liste_points_voisins** mat_points_voisins);


//Calcule les valeurs du système d'équations différentielles au temps "d" en utilisant la méthode d'Euler.
// - mat_points_voisins: matrice des listes des points voisins pour chaque cellule
// - d: jour courant
// - tx_ret: taux de rétablissement
void euler(cellule** automate, liste_points_voisins** mat_points_voisins, int d, float tx_ret);


//Ajoute la proportion des infectés de chaque cellule au temps d dans la matrice "somme_inf"
void somme_inf_aut(cellule** automate, double** somme_inf, int d);


//Renvoi le maximum de la matrice des proportion des infectées sommés "somme_inf"
double trouver_max_somme_inf(double** somme_inf);


#endif
#ifndef _VAR_H_
#define _VAR_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "utils.h"

//Paramètres du système :
extern double tx_inf; //Taux d'infection
extern double tx_ret; //Taux de retablissement
extern int rayon_propagation; //Rayon de propagation definissant l'ensemble des cellules voisines
extern int nombre_cellules;

extern int nombre_jours; //Nombre de jours totaux


//Matrice de densité (en cas de graymap)
extern double (*fonction_dens)(double, double); //fonction de densité
extern bool afficher_dens; //Afficher la matrice de densité
extern char* chemin_dens; //chemin d'acces de la graymap en .pgm (NULL en cas de fonction de densité)


//Afficher les proportions d'infectés sommés jusqu'au temps d au lieu des proportions du temps d
extern double afficher_somme_inf;

//Variables graphiques
extern int marge_graph;
extern SDL_Color c_noir;
extern SDL_Color c_gris;
extern SDL_Color c_gris_fonce;
extern SDL_Color c_vert;
extern SDL_Color c_bleu;
extern SDL_Color c_magenta;
extern SDL_Color c_jaune_fonce;
extern SDL_Color c_blanc;

#endif
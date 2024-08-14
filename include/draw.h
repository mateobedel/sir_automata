#ifndef _DRAW_H_
#define _DRAW_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "automata.h"

#define LARGEUR 600
#define HAUTEUR 600


#define SAIN 0
#define INFECTE 1
#define RETABLI 2

//Renvoi la couleur correspondant à "pourcentage" du dégradé de couleurs "couleurs"
SDL_Color couleur_degrade(SDL_Color* couleurs, int n, double pourcentage);


//Initialise les fonctions du graphiques
SDL_Point** fonctions_init(int nombre_jours);


//Convertit les données de l'automate en coordonnées pour le graph au temps d
// - fonctions: images des fonctions de population globale du système (somme des fonctions de population de chaque cellule proportionnement à leur densité de population)
// - d: jour courant
void aut_vers_fonctions(SDL_Point** fonctions, cellule** automate, int d, int nombre_jours, int originX, int maxX, int originY, int maxY);


//Trace une ligne pour le graphique
// - points : liste des points de la ligne
// - longueur : nombre de points dans la ligne
void dessine_ligne_epaisse(SDL_Renderer* renderer, SDL_Point* points, int longueur);


//Trace un carré pour le graphique
void dessine_carre(SDL_Renderer* renderer, int x1, int y1, int x2, int y2);


//Definitit la bonne texture et le bon rectangle pour afficher un texte
void init_texte(SDL_Renderer *renderer, int x, int y, char *text, SDL_Color couleur, TTF_Font *police, SDL_Texture **texture, SDL_Rect *rect);


//Affiche les textes du graphique
// - temps_num : nombre de valeurs en abscisse
// - prop_num : nombre de valeurs en ordonnée
void dessine_textes_graph(SDL_Renderer* renderer, int temps_num, int prop_num, int nombre_jours, SDL_Texture** temps_textes, SDL_Rect* temps_rectangles, SDL_Texture** prop_textes, SDL_Rect* prop_rectangles, int marge_graph, TTF_Font *police, SDL_Color couleur);


//Trace le graphique
void dessine_graph(SDL_Renderer* renderer, int originX, int originY, int maxX, int maxY,  SDL_Texture** temps_textes, SDL_Texture** prop_textes,SDL_Rect* temps_rectangles, SDL_Rect* prop_rectangles, int temps_num, int prop_num, int d, int nombre_jours, SDL_Color couleur, SDL_Color color2,SDL_Color color3, TTF_Font* police, char* msg, int calc_jours);


//Affiche l'automate cellulaire au temps d
// - d: jour courant
void dessine_aut(SDL_Renderer* renderer, cellule** automate,  int d, SDL_Color* degrade, int nombre_couleurs);


//Affiche l'automate cellulaire de toutes les proportions d'infectés des cellules sommés jusqu'au temps d
// - d: jour courant
// - max_somme_inf: maximum de la matrice des proportion des infectées sommés 
void dessine_aut_somme_inf(SDL_Renderer* renderer, cellule** automate,  int d, SDL_Color* degrade, int nombre_couleurs, double max_somme_inf);


//Affiche la martice de densité (graymap)
void dessine_dens(SDL_Renderer* renderer, PGMData* pgm_dens);


//Trace une fonction
void dessine_fonctions(SDL_Renderer* renderer, SDL_Point* points, int longueur, SDL_Color couleur);

#endif
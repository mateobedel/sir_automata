
#include "var.h"

//Paramètres du système :
double tx_inf = .1; //Taux d'infection
double tx_ret = .05; //Taux de retablissement
int rayon_propagation = 3; //Rayon de propagation definissant l'ensemble des cellules voisines
int nombre_jours = 200; //Nombre de jours totaux
int nombre_cellules = 100; //Longueur/largeur de l'automate cellulaire

//Les deux méthodes de répartitions de la densité (fonction ou graymap)
double (*fonction_dens)(double, double) = f_const; //fonction de densité (active que si chemin_dens vaut NULL) (f_const ou f_gauss)
char* chemin_dens = "graymap/densnocompr.pgm"; //chemin d'acces de la graymap en .pgm 

//Afficher les proportions d'infectés sommés jusqu'au temps d au lieu des proportions du temps d
double afficher_somme_inf = false;

//Variables graphiques
bool afficher_dens = false; //Afficher la matrice de densité
int marge_graph = 100;
SDL_Color c_noir = {.r = 0, .g = 0, .b = 0};
SDL_Color c_gris = {.r = 220, .b = 220, .g = 220};
SDL_Color c_gris_fonce = {.r = 60, .b = 60, .g = 60};
SDL_Color c_vert = {.r = 0, .b = 0, .g = 255};
SDL_Color c_bleu = {.r = 0, .g = 0, .b = 255};
SDL_Color c_magenta = {.r = 255, .g = 0, .b = 30};
SDL_Color c_jaune_fonce = {.r = 255, .g = 166, .b = 0};
SDL_Color c_blanc= {.r = 255, .g = 255, .b = 255};

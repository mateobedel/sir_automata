#include "automata.h"



//Génère une liste de points voisins dans un disque de propagation donné autour des coordonnées (xc, yc)
// - tx_inf: paramètre d'infection du système
liste_points_voisins points_disque_propagation(int rayon, int xc, int yc, cellule** automate, float tx_inf) {

    //Liste des lignes à tracer
    liste_points_voisins l;
    l.nombre = 0;
    int n_points = (rayon+1)*(rayon+1)*4;  
    l.liste_points = malloc(sizeof(point_voisin)*n_points);

    for(int i =-rayon; i <= rayon; i++) {
        for(int j=-rayon; j <= rayon; j++) {
            if(i*i+j*j <= rayon*rayon) { //(i, j) est à l'intérieur du disque de propagation :
                if (xc + i >= 0 && yc + j >= 0 && xc + i < nombre_cellules && yc + j < nombre_cellules) {
                     l.liste_points[l.nombre].x = xc + i; // Coordonnée x du point voisin
                     l.liste_points[l.nombre].y = yc + j; // Coordonnée y du point voisin

                     //Calcul du taux d'infection entre (xc,yc) et la cellule voisine en fonction de la densité
                     l.liste_points[l.nombre].tx_inf_voisin = tx_inf*sqrt(automate[xc+i][yc+j].dens*automate[xc][yc].dens)/(1+sqrt(i*i+j*j));

                     l.nombre++; // Augmentation du nombre de points voisins dans la liste
                }
            }
        }
    }
    return l;
}


//Initialise la grille de cellules dans l'automate cellulaire avec des valeurs initiales.
// - pgm_dens: pointeur vers une structure PGMData contenant une graymap de densité (peut être NULL)
// - f: fonction de densité (si il n'y a pas de matrice de densité)
cellule** aut_init(int nombre_jours, double (*f)(double, double), PGMData* pgm_dens) {
    
    //Allocation
    cellule** automate = malloc(sizeof(cellule*)*nombre_cellules);
    for (int i = 0; i < nombre_cellules; i++)
        automate[i] = malloc(sizeof(cellule)*nombre_cellules);

    if (pgm_dens != NULL) {
        if (pgm_dens->nx != nombre_cellules || pgm_dens->ny != nombre_cellules) {
            printf("Erreur : mauvaise dimension d'image\n");
            exit(1);
        }
    }

    //Condition initiale du système
    for (int i = 0; i < nombre_cellules; i++) {
        for (int j = 0; j < nombre_cellules; j++) {
            
            automate[i][j].sain = malloc(sizeof(double)*nombre_jours);
            automate[i][j].inf = malloc(sizeof(double)*nombre_jours);
            automate[i][j].ret = malloc(sizeof(double)*nombre_jours);
            
            //Enregistrement des valeurs de la matrice/fonction de densité dans la strucure de l'automate
            if (pgm_dens == NULL) {
                automate[i][j].dens = f(((double)i-(double)nombre_cellules/2)/((double)nombre_cellules/2),((double)j-(double)nombre_cellules/2)/((double)nombre_cellules/2));
            } else {
                automate[i][j].dens = ((double)pgm_dens->t[j][i])/((double)pgm_dens->max_gris);
            }

            automate[i][j].sain[0] = 1 ;
            automate[i][j].inf[0] = 0;
        }
    }
    
    //Infectés initiaux au milieu de l'automate
    automate[nombre_cellules/4][nombre_cellules/4].sain[0] = 0;
    automate[nombre_cellules/4][nombre_cellules/4].inf[0] = 1;

    

    return automate; //matrice représentant l'automate cellulaire initialisée
}


//Free l'automate
void aut_free(cellule** automate) {
    for (int i = 0; i < nombre_cellules; i++) {
        for (int j = 0; j < nombre_cellules; j++) {
            free(automate[i][j].sain);
            free(automate[i][j].inf);
            free(automate[i][j].ret);
        }
        free(automate[i]);
    }
    free(automate);
}


//Calcule la matrice des listes des cellules voisines pour chaque cellule dans l'automate.
// - rayon_propagation: rayon de propagation des cellules voisines
// - tx_inf: facteur d'infection
liste_points_voisins** cellules_voisines(cellule** automate, float tx_inf, int rayon_propagation) {
    //Calcul de la matrice des points voisins
    liste_points_voisins** mat_points_voisins = malloc(sizeof(liste_points_voisins*)*nombre_cellules);
    for(int i=0; i < nombre_cellules; i++) mat_points_voisins[i] = malloc(sizeof(liste_points_voisins)*nombre_cellules);

    for(int i = 0; i < nombre_cellules; i++) 
        for(int j = 0; j < nombre_cellules; j++)
            mat_points_voisins[i][j] = points_disque_propagation(rayon_propagation, i, j, automate, tx_inf); // Calcul des points voisins pour chaque cellule en utilisant la fonction points_disque_propagation

    return mat_points_voisins; //matrice des listes des points voisins pour chaque cellule
}


//Free les celulles voisines
void free_cellules_voisines(liste_points_voisins** mat_points_voisins) {
    for(int i=0; i < nombre_cellules; i++) {
        for(int j=0; j < nombre_cellules; j++) {
            free(mat_points_voisins[i][j].liste_points);
        }
        free(mat_points_voisins[i]);
    }
    free(mat_points_voisins);
}


//Calcule les valeurs du système d'équations différentielles au temps "d" en utilisant la méthode d'Euler.
// - mat_points_voisins: matrice des listes des points voisins pour chaque cellule
// - d: jour courant
// - tx_ret: taux de rétablissement
void euler(cellule** automate, liste_points_voisins** mat_points_voisins, int d, float tx_ret) {

    for(int i = 0; i < nombre_cellules; i++) {
        for(int j = 0; j < nombre_cellules; j++) {

            // Population au temps d-1 :
            double pSain = automate[i][j].sain[d-1];
            double pInf = automate[i][j].inf[d-1];
            double pRet = automate[i][j].ret[d-1];

            //Variation de la population au sein de la cellule (Méthode d'euler):
            automate[i][j].sain[d] = pSain;
            automate[i][j].inf[d] = pInf - tx_ret*pInf;
            automate[i][j].ret[d] = pRet + tx_ret*pInf; 

            //Variation des populations entre la cellule (i,j) et les cellules voisines v
            for(int v = 0; v < mat_points_voisins[i][j].nombre; v++) {
                
                double tx_inf_voisin = mat_points_voisins[i][j].liste_points[v].tx_inf_voisin; //Taux d'infection entre la cellule (i,j) et v
                //Coordonnées de la cellule voisine v :
                int vx = mat_points_voisins[i][j].liste_points[v].x;
                int vy = mat_points_voisins[i][j].liste_points[v].y;
                //Population infectés de v au temps d-1
                double pInf_voisins = automate[vx][vy].inf[d-1];

                //Méthode d'euler
                automate[i][j].sain[d] += -tx_inf_voisin*pSain*pInf_voisins;
                automate[i][j].inf[d] += tx_inf_voisin*pSain*pInf_voisins;
            }

            //Limite les population entre 0 et 1 pour eviter les erreurs du compileur des operations entres les double
            automate[i][j].sain[d] = clamp(automate[i][j].sain[d], 0, 1);
            automate[i][j].inf[d] = clamp(automate[i][j].inf[d], 0, 1);
            automate[i][j].ret[d] = clamp(automate[i][j].ret[d], 0, 1);
        }
    }
}


//Ajoute la proportion des infectés de chaque cellule au temps d dans la matrice "somme_inf"
void somme_inf_aut(cellule** automate, double** somme_inf, int d) {   
    for (int i=0; i < nombre_cellules; i++) 
        for(int j=0; j < nombre_cellules; j++) 
            somme_inf[i][j] += automate[i][j].inf[d];  
}


//Renvoi le maximum de la matrice des proportion des infectées sommés "somme_inf"
double trouver_max_somme_inf(double** somme_inf) {
    double max_v = 0;
    for (int i=0; i < nombre_cellules; i++) 
        for(int j=0; j < nombre_cellules; j++) 
            if (somme_inf[i][j] > max_v) max_v = somme_inf[i][j];
    return max_v;
}
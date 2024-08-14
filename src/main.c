#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "graymap.h"
#include "automata.h"
#include "utils.h"
#include "draw.h"
#include "var.h"


int main(void) {

    bool en_cours = true; //variable d'execution
    TTF_Init(); //initalisation des polices

    int max_somme_inf = 0; //Maximum de la matrice de la somme des proportions d'infectés
    
    int mod_nombre_jours = nombre_jours; //Nombre de jours modifiés par le gestionnaire des evenements
    int calc_jours = 1; //Jours calculés par la modelisation
    int d = 0; //Jour de la modelisation qu'affiche le programme


    PGMData* pgm_dens = NULL; //Structure de la graymap
    //Lecture de l'image :
    if (chemin_dens !=NULL) {
        pgm_dens = malloc(sizeof(PGMData));
        lirePGM(chemin_dens, pgm_dens);
    }

    //Initialisation de l'automate
    cellule** automate = aut_init(nombre_jours, fonction_dens, pgm_dens); //condition initiale
    liste_points_voisins** mat_points_voisins = cellules_voisines(automate,tx_inf, rayon_propagation); //calcul des points voisins

    //Tableau de la sommes des proportions d'infectés de chaque cellule
    double** somme_inf;
    if (afficher_somme_inf) { //Si l'affichage est réglé sur la somme :
        somme_inf = malloc(sizeof(double*)*nombre_cellules);
        for (int i=0; i < nombre_cellules; i++) somme_inf[i] = malloc(sizeof(double)*nombre_cellules);
    
    //Initialisation :
    for (int i=0; i < nombre_cellules; i++)
        for (int j=0; j < nombre_cellules; j++)
            somme_inf[i][j] = 0;
    }

    //Dégradé de couleurs d'affichage pour l'automate:
    SDL_Color* degrade = malloc(sizeof(SDL_Color)*3);
    degrade[0] = c_blanc; degrade[1] = c_jaune_fonce; degrade[2] = c_magenta;
    int nombre_couleurs = 3;


   //Gerer erreur d'initialisation
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur: SDL ne peut pas s'initialiser\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    //Recuperer le displaymode pour l'affichage
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        SDL_Log("Erreur de recuperation du displaymode: %s", SDL_GetError());
        return 1;
    }
    int largeur_ecran, hauteur_ecran; largeur_ecran = dm.w; hauteur_ecran = dm.h; //donnees de l'ecran

    //Creation des fenetres
    SDL_Window* window = SDL_CreateWindow("SIR", (largeur_ecran-(2*LARGEUR))/3, (hauteur_ecran-HAUTEUR)/2, LARGEUR, HAUTEUR, 0);
    SDL_Window* window_graph = SDL_CreateWindow("Courbes", (largeur_ecran-(2*LARGEUR))*2/3+LARGEUR, (hauteur_ecran-HAUTEUR)/2, LARGEUR, HAUTEUR, 0);
    if (!window || !window_graph) {
        //Gerer erreur d'ouverture de fenetre
        printf("Erreur : SDL n'arrive pas à ouvrir une fenetre\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }
    //Creation du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Renderer* renderer_graph = SDL_CreateRenderer(window_graph, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer || !renderer_graph) {
        //Gerer erreur de creation du renderer
        printf("Erreur : Echec de la création des renderers\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    //Variables de textes
    char* chemin_police = "font/Poppins-Medium.ttf"; //chemin de la police
    TTF_Font *police = TTF_OpenFont(chemin_police, 14);
    if (police == NULL) printf("TTF_Init: %s\n", TTF_GetError()); //Gerer erreur de lecture de la police
    int maxmsg = 100; //Taille du message d'affichage maximum
    char* msg = malloc(sizeof(char)*maxmsg);
    for(int i = 0; i < maxmsg; i++) msg[i] = '\0'; //Initialisation
    SDL_Keycode touche_presse = SDLK_i; //Touche selectionne intiale
    snprintf(msg,maxmsg,"[!] Touche selectionnee : %c",touche_presse);

    //Assignation des pointeur des polices
    int temps_num = 10; //Nombre de valeurs de l'abscisse
    int prop_num = 10; //Nombre de valeurs de l'ordonnée
    SDL_Texture** temps_textes = malloc(sizeof(SDL_Texture*)*(temps_num+1));
    SDL_Rect* temps_rectangles = malloc(sizeof(SDL_Rect)*(temps_num+1));
    SDL_Texture** prop_textes = malloc(sizeof(SDL_Texture*)*(prop_num+1));
    SDL_Rect* prop_rectangles = malloc(sizeof(SDL_Rect)*(prop_num+1));

    //Courbes du graphique
    dessine_textes_graph(renderer_graph,temps_num,prop_num,nombre_jours,temps_textes,temps_rectangles,prop_textes,prop_rectangles,marge_graph,police,c_gris_fonce);
    SDL_Point** fonctions = fonctions_init(nombre_jours);
    aut_vers_fonctions(fonctions,automate, 0, nombre_jours, marge_graph, LARGEUR-marge_graph, marge_graph, HAUTEUR-marge_graph);
    
    while (en_cours) { //Execution

        //Resolution du système en temps-réel
        if (calc_jours < nombre_jours) {
            euler(automate, mat_points_voisins, calc_jours, tx_ret);
            aut_vers_fonctions(fonctions,automate, calc_jours, nombre_jours, marge_graph, LARGEUR-marge_graph, marge_graph, HAUTEUR-marge_graph);
            if (afficher_somme_inf)  { 
                somme_inf_aut(automate,somme_inf, calc_jours);
                max_somme_inf = trouver_max_somme_inf(somme_inf);
            }
            calc_jours++; //Augmentation des jours calculés
        }

        //Initialisation des renderers
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer_graph, 255, 255, 255, 1);
        SDL_RenderClear(renderer_graph);

        //Affichage du graphique
        dessine_graph(renderer_graph,marge_graph,marge_graph,LARGEUR-marge_graph,HAUTEUR-marge_graph,temps_textes, prop_textes, temps_rectangles, prop_rectangles, temps_num, prop_num,d,nombre_jours,c_noir,c_gris,c_magenta, police, msg,calc_jours);
        dessine_fonctions(renderer_graph, fonctions[SAIN], calc_jours, c_vert); //Fonction des individus sains
        dessine_fonctions(renderer_graph, fonctions[INFECTE], calc_jours, c_jaune_fonce); //Fonction des individus infectés
        dessine_fonctions(renderer_graph, fonctions[RETABLI], calc_jours, c_bleu); //Fonction des individus retablis

        //Affichage de l'automate (Propotions d'infectés sommés ou proportion d'infectés au temps d)
        if (!afficher_somme_inf) dessine_aut(renderer, automate, d, degrade, nombre_couleurs);
        else dessine_aut_somme_inf(renderer, automate, d, degrade, nombre_couleurs,max_somme_inf);

        //Affichage de la matrice de densité
        if (afficher_dens && chemin_dens != NULL) dessine_dens(renderer,pgm_dens);
        SDL_SetRenderDrawColor(renderer,0,0,0,1);

        //Gestionnaire d'evenements
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_KEYDOWN: //Baisser une valeur
                    switch(event.key.keysym.sym) {

                        //Changer le jour courant d (<- / ->)
                        case SDLK_RIGHT: if (d<calc_jours-1) d++; //Augmenter (->)
                        break;
                        case SDLK_LEFT: if (d>0) d--; //Diminuer (<-)
                        break;
                        //Aficher la matrice de densité (D)
                        case SDLK_d: 
                            afficher_dens = true;
                        break;

                        //Augmenter les paramètres de la simulation
                        case SDLK_UP: //Augmenter (flèche du haut)
                            switch(touche_presse){
                                case SDLK_i : tx_inf = clamp(tx_inf+.01,0,1); snprintf(msg,maxmsg,"[!] Taux d'infection = %lf",tx_inf); break; //Taux d'infection (I)
                                case SDLK_r : tx_ret = clamp(tx_ret+.01,0,1); snprintf(msg,maxmsg,"[!] Taux de retablissement  = %lf",tx_ret); break; //Taux de retablissement (R)
                                case SDLK_j : mod_nombre_jours = clamp(mod_nombre_jours+50,50,1000); snprintf(msg,maxmsg,"[!] Jours = %d",mod_nombre_jours); break; //Nombre de jour (J)
                            }
                        break;
                        case SDLK_DOWN: //Diminuer (flèche du bas)
                            switch(touche_presse){
                                case SDLK_i : tx_inf = clamp(tx_inf-.01,0,1); snprintf(msg,maxmsg,"[!] Taux d'infection = %lf",tx_inf); break; //Taux d'infection (I)
                                case SDLK_r : tx_ret = clamp(tx_ret-.01,0,1); snprintf(msg,maxmsg,"[!] Taux de retablissement   = %lf",tx_ret); break; //Taux de retablissement (R)
                                case SDLK_j : mod_nombre_jours = clamp(mod_nombre_jours-50,50,1000); snprintf(msg,maxmsg,"[!] Jours = %d",mod_nombre_jours); break; //Nombre de jour (J)
                            }
                        break;
                        
                        //Changer le paramètre selectionné
                        case SDLK_i : case SDLK_r : case SDLK_j : 
                            touche_presse = event.key.keysym.sym;
                            snprintf(msg,maxmsg,"[!] Touche selectionnee : %c",touche_presse);
                        break;

                        //Actualiser la simulation
                        case SDLK_RETURN:
                            //Reinitialisation de toute les valeurs initiales :
                            free_cellules_voisines(mat_points_voisins);
                            aut_free(automate);
                            for(int i = 0; i < 3; i++) free(fonctions[i]);
                            free(fonctions);

                            d = 0;
                            nombre_jours = mod_nombre_jours;
                            calc_jours = 1;

                            max_somme_inf = 0;
                            if (afficher_somme_inf)
                                for (int i=0; i < nombre_cellules; i++)
                                    for (int j=0; j < nombre_cellules; j++)
                                        somme_inf[i][j] = 0;
                            
                            automate = aut_init(nombre_jours, f_const, pgm_dens);
                            mat_points_voisins = cellules_voisines(automate,tx_inf, rayon_propagation);
                            dessine_textes_graph(renderer_graph,temps_num,prop_num,nombre_jours,temps_textes,temps_rectangles,prop_textes,prop_rectangles,marge_graph,police,c_gris_fonce);
                            fonctions = fonctions_init(nombre_jours);
                            aut_vers_fonctions(fonctions,automate, 0, nombre_jours, marge_graph, LARGEUR-marge_graph, marge_graph, HAUTEUR-marge_graph);
                            snprintf(msg,maxmsg,"[!] Donnees mises a jour.");
                        break;
                    }
                break;

                case SDL_KEYUP:
                    switch(event.key.keysym.sym) {
                        case SDLK_d: //Afficher la matrice de densité (D)
                            afficher_dens = false;
                        break;

                    }

            }
            //Fermer la fenetre
            if ((event.type==SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                en_cours = false;
            }
        }
        //Mise a jour du renderer
        SDL_RenderPresent(renderer);
        SDL_RenderPresent(renderer_graph); 

        
    }

    //Free des pointeurs
    free_cellules_voisines(mat_points_voisins);
    for(int i = 0; i < 3; i++) free(fonctions[i]);
         free(fonctions);
    if (afficher_somme_inf) {
        for (int i =0; i < nombre_cellules; i++) free(somme_inf[i]);
        free(somme_inf);
    }
    TTF_CloseFont(police);
    aut_free(automate);
    free(msg);
    free(degrade);
    freePGM(pgm_dens);
    for(int i= 0; i < temps_num; i++) SDL_DestroyTexture(temps_textes[i]);
    free(temps_textes);
    for(int i= 0; i < prop_num; i++) SDL_DestroyTexture(prop_textes[i]);
    free(prop_textes);
    free(temps_rectangles);
    free(prop_rectangles);
    return 0;
}

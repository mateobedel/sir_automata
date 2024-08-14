#include "draw.h"


//Renvoi la couleur correspondant à "pourcentage" du dégradé de couleurs "couleurs"
SDL_Color couleur_degrade(SDL_Color* couleurs, int n, double pourcentage) {
    if (n == 1) return couleurs[0];
    pourcentage = clamp(pourcentage, 0, 1);
    int pas_couleur = min(floor(pourcentage*(n-1)),n-2);
    double pourcentage_rgb = map(pourcentage,(double)(pas_couleur)/(n-1),(double)(pas_couleur+1)/(n-1),0.f,1.f);

    SDL_Color couleur_actuelle;
    couleur_actuelle.r = (int)(couleurs[pas_couleur].r * (1-pourcentage_rgb) + couleurs[pas_couleur+1].r * pourcentage_rgb);
    couleur_actuelle.g = (int)(couleurs[pas_couleur].g * (1-pourcentage_rgb) + couleurs[pas_couleur+1].g * pourcentage_rgb);
    couleur_actuelle.b = (int)(couleurs[pas_couleur].b * (1-pourcentage_rgb) + couleurs[pas_couleur+1].b * pourcentage_rgb);

    return couleur_actuelle;

}


//Initialise les fonctions du graphiques
SDL_Point** fonctions_init(int nombre_jours) {

    SDL_Point** fonctions = malloc(sizeof(SDL_Point)*3);
    for(int i=0; i < 3; i++) fonctions[i] = malloc(sizeof(SDL_Point)*nombre_jours);

    return fonctions;
}


//Convertit les données de l'automate en coordonnées pour le graph au temps d
// - fonctions: images des fonctions de population globale du système (somme des fonctions de population de chaque cellule proportionnement à leur densité de population)
// - d: jour courant
void aut_vers_fonctions(SDL_Point** fonctions, cellule** automate, int d, int nombre_jours, int originX, int maxX, int originY, int maxY) {
    
    //Densité totale de population
    float sain_tot, inf_tot, ret_tot, dens_tot;
    dens_tot = 0;
    for (int i =0; i < nombre_cellules; i++) {
        for (int j =0; j < nombre_cellules; j++) {
            dens_tot += automate[i][j].dens;
        }
    }
    //Calcul des valeurs de chaque cellules sommées proportionnelement a leur densité de population
    sain_tot = inf_tot = ret_tot = 0;
    for (int i =0; i < nombre_cellules; i++) {
        for (int j =0; j < nombre_cellules; j++) {
            sain_tot += automate[i][j].sain[d]*automate[i][j].dens;
            inf_tot += automate[i][j].inf[d]*automate[i][j].dens;
            ret_tot += automate[i][j].ret[d]*automate[i][j].dens;
        }
    }

    //Enregistrement des ordonnées      
    fonctions[SAIN][d].x = fonctions[INFECTE][d].x = fonctions[RETABLI][d].x = originX + d*(maxX - originX)/nombre_jours;
    fonctions[SAIN][d].y = HAUTEUR - originY - sain_tot/(dens_tot)*(maxY - originY);
    fonctions[INFECTE][d].y = HAUTEUR - originY - inf_tot/(dens_tot)*(maxY - originY);
    fonctions[RETABLI][d].y = HAUTEUR - originY - ret_tot/(dens_tot)*(maxY - originY);
}


//Trace une ligne pour le graphique
// - points : liste des points de la ligne
// - longueur : nombre de points dans la ligne
void dessine_ligne_epaisse(SDL_Renderer* renderer, SDL_Point* points, int longueur) {
    for(int i = 0; i < longueur-1; i++) {
        SDL_RenderDrawLine(renderer,points[i].x,points[i].y,points[i+1].x,points[i+1].y);
        SDL_RenderDrawLine(renderer,points[i].x-1,points[i].y,points[i+1].x-1,points[i+1].y);
        SDL_RenderDrawLine(renderer,points[i].x,points[i].y-1,points[i+1].x,points[i+1].y-1);
    }
}


//Trace un carré pour le graphique
void dessine_carre(SDL_Renderer* renderer, int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y1);
    SDL_RenderDrawLine(renderer, x2, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x1, y2);
    SDL_RenderDrawLine(renderer, x1, y2, x1, y1);
}


//Definitit la bonne texture et le bon rectangle pour afficher un texte
void init_texte(SDL_Renderer *renderer, int x, int y, char *text, SDL_Color couleur, TTF_Font *police, SDL_Texture **texture, SDL_Rect *rect) {
    int largeur_texte;
    int hauteur_texte;
    SDL_Surface *surface;

    surface = TTF_RenderText_Solid(police, text, couleur);
    if (surface == NULL) return;
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    largeur_texte = surface->w;
    hauteur_texte = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = largeur_texte;
    rect->h = hauteur_texte;
}


//Affiche les textes du graphique
// - temps_num : nombre de valeurs en abscisse
// - prop_num : nombre de valeurs en ordonnée
void dessine_textes_graph(SDL_Renderer* renderer, int temps_num, int prop_num, int nombre_jours, SDL_Texture** temps_textes, SDL_Rect* temps_rectangles, SDL_Texture** prop_textes, SDL_Rect* prop_rectangles, int marge_graph, TTF_Font *police, SDL_Color couleur) {
    //Temps
    for(int i=0; i <= temps_num; i++) {
        int y = HAUTEUR - marge_graph + 5;
        int x = marge_graph + i*(LARGEUR-2*marge_graph)/(temps_num);
        int n = i*nombre_jours/(temps_num);

        char* str1 = malloc(sizeof(char)*(nombre_chiffres(n)+1));
        snprintf(str1,nombre_chiffres(n)+1,"%d",n);

        init_texte(renderer, x, y, str1, couleur, police, &(temps_textes[i]), &(temps_rectangles[i]));
        free(str1);
    }

    //Pourcentage
    for(int i = 0; i <= prop_num; i++) {
        int x = marge_graph - 5;
        int y = marge_graph + i*(HAUTEUR-2*marge_graph)/prop_num;
        int n = i*100/prop_num;
        int* x_align_droite = malloc(sizeof(int));
        
        char* str2 = malloc(sizeof(char)*(nombre_chiffres(n)+3));
        snprintf(str2,nombre_chiffres(n)+3,"%d %%",n);
        TTF_SizeText(police,str2, x_align_droite, NULL);

        init_texte(renderer, x-(*x_align_droite), HAUTEUR-y, str2, couleur, police, &(prop_textes[i]), &(prop_rectangles[i]));
        free(x_align_droite);
        free(str2);
    }
}

//Trace le graphique
void dessine_graph(SDL_Renderer* renderer, int originX, int originY, int maxX, int maxY,  SDL_Texture** temps_textes, SDL_Texture** prop_textes,SDL_Rect* temps_rectangles, SDL_Rect* prop_rectangles, int temps_num, int prop_num, int d, int nombre_jours, SDL_Color couleur, SDL_Color color2,SDL_Color color3, TTF_Font* police, char* msg, int calc_jours) {
    //Lignes Verticales
    for(int i=0; i <= temps_num; i++) {
        SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
        SDL_RenderCopy(renderer, temps_textes[i], NULL, &temps_rectangles[i]);
        SDL_SetRenderDrawColor(renderer, color2.r, color2.g, color2.b, color2.a);
        SDL_RenderDrawLine(renderer, originX+i*(maxX-originX)/temps_num,originY,originX+i*(maxX-originX)/temps_num,maxY);
    }
    
    //Lignes horizontales
    for(int i = 1; i <= prop_num; i++) {
        SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
        SDL_RenderCopy(renderer, prop_textes[i], NULL, &prop_rectangles[i]);
        SDL_SetRenderDrawColor(renderer, color2.r, color2.g, color2.b, color2.a);
        SDL_RenderDrawLine(renderer, originX, originY+i*(maxY-originY)/temps_num,maxX,originY+i*(maxY-originY)/temps_num);
    }
    
    //Message affiché en haut du graphique
    SDL_Texture* text_msg;
    SDL_Rect rect_msg;
    init_texte(renderer, 50, 30, msg, couleur, police, &text_msg, &rect_msg);
    SDL_RenderCopy(renderer, text_msg, NULL, &rect_msg);
    SDL_DestroyTexture(text_msg);

    //Affiche le nombre de jours
    int taille_calc = nombre_chiffres(calc_jours) + nombre_chiffres(nombre_jours) + 10; //39/200 jours
    char* str_calc = malloc(sizeof(char)*taille_calc);
    snprintf(str_calc,taille_calc,"%d/%d jours",calc_jours,nombre_jours);
    SDL_Texture* text_calc;
    SDL_Rect rect_calc;
    init_texte(renderer, maxX-10, maxY+70, str_calc, couleur, police, &text_calc, &rect_calc);
    SDL_RenderCopy(renderer, text_calc, NULL, &rect_calc);
    SDL_DestroyTexture(text_calc);
    free(str_calc);


    //Affiche le jour courant
    int taille_d = nombre_chiffres(d) + 5; //d = %d
    char* str_d = malloc(sizeof(char)*taille_d);
    snprintf(str_d,taille_d,"d = %d",d);
    SDL_Texture* text_d;
    SDL_Rect rect_d;
    init_texte(renderer, (int)(originX+(float)d/nombre_jours*(maxX-originX)), originY-20, str_d, color3, police, &text_d, &rect_d);
    SDL_RenderCopy(renderer, text_d, NULL, &rect_d);
    SDL_DestroyTexture(text_d);
    free(str_d);

    //Ligne verticale du jour courant
    SDL_SetRenderDrawColor(renderer, color3.r, color3.g, color3.b, color3.a);
    SDL_RenderDrawLine(renderer, originX+(float)d/nombre_jours*(maxX-originX), HAUTEUR-originY, originX+(float)d/nombre_jours*(maxX-originX), originY);
    SDL_RenderDrawLine(renderer, originX+(float)d/nombre_jours*(maxX-originX)+1, HAUTEUR-originY, originX+(float)d/nombre_jours*(maxX-originX)+1, originY);

    //Cadre
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    dessine_carre(renderer, originX, originY, maxX, maxY);
}

//Affiche l'automate cellulaire au temps d
// - d: jour courant
void dessine_aut(SDL_Renderer* renderer, cellule** automate,  int d, SDL_Color* degrade, int nombre_couleurs) {

    int taille_cellule = LARGEUR/nombre_cellules;

    for (int i=0; i < nombre_cellules; i++) {
        for(int j=0; j < nombre_cellules; j++) {
            SDL_Rect c;
            c.x = taille_cellule*i;
            c.y = taille_cellule*j;
            c.w = taille_cellule;
            c.h = taille_cellule;

            //Calcul la bonne couleur du dégradé relativement à la proportion d'infectés de la cellule
            SDL_Color g_couleur = couleur_degrade(degrade, nombre_couleurs, automate[i][j].inf[d]);
            SDL_SetRenderDrawColor(renderer, g_couleur.r, g_couleur.g, g_couleur.b, 1);
            SDL_RenderFillRect(renderer, &c);
        }
    }

}


//Affiche l'automate cellulaire de toutes les proportions d'infectés des cellules sommés jusqu'au temps d
// - d: jour courant
// - max_somme_inf: maximum de la matrice des proportion des infectées sommés 
void dessine_aut_somme_inf(SDL_Renderer* renderer, cellule** automate,  int d, SDL_Color* degrade, int nombre_couleurs, double max_somme_inf) {

    int taille_cellule = LARGEUR/nombre_cellules;

    for (int i=0; i < nombre_cellules; i++) {
        for(int j=0; j < nombre_cellules; j++) {
            SDL_Rect c;
            c.x = taille_cellule*i;
            c.y = taille_cellule*j;
            c.w = taille_cellule;
            c.h = taille_cellule;

            //Somme des proportions d'infectés jusqu'au temps d
            double somme = 0;
            for (int t = 0; t <= d; t++) somme += automate[i][j].inf[t];

            //Rapport entre la somme des infecté de la cellule jusqu'au temps d et la somme maximal des autres cellules
            double rapport;
            if (max_somme_inf == 0) rapport = 0;
            else rapport = somme/max_somme_inf;

             //Calcul la bonne couleur du dégradé relativement à la proportion d'infectés de la cellule
            SDL_Color g_couleur = couleur_degrade(degrade, nombre_couleurs, rapport);
            SDL_SetRenderDrawColor(renderer, g_couleur.r, g_couleur.g, g_couleur.b, 1);
            SDL_RenderFillRect(renderer, &c);
        }
    }

}

//Affiche la martice de densité (graymap)
void dessine_dens(SDL_Renderer* renderer, PGMData* pgm_dens) {
    if (pgm_dens == NULL) return;

   int taille_cellule = LARGEUR/nombre_cellules;
    for (int i=0; i < nombre_cellules; i++) {
        for(int j=0; j < nombre_cellules; j++) {
            SDL_Rect c;
            c.x = taille_cellule*i;
            c.y = taille_cellule*j;
            c.w = taille_cellule;
            c.h = taille_cellule;
            int gris = pgm_dens->t[j][i]*255/pgm_dens->max_gris;
            
            SDL_SetRenderDrawColor(renderer, gris, gris, gris, 1);
            SDL_RenderFillRect(renderer, &c);
        }
    } 
}

//Trace une fonction
void dessine_fonctions(SDL_Renderer* renderer, SDL_Point* points, int longueur, SDL_Color couleur) {
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    dessine_ligne_epaisse(renderer, points, longueur);
}

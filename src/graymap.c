#include "graymap.h"

//Lit une image .pgm (graymap) puis renvoi la matrice de densité associé
void lirePGM(char* dir, PGMData* data) {

FILE* input = fopen(dir,"r");
    if (input == NULL) {
        printf("Erreur à l'ouverture du fichier\n");
        exit(1);
    }

    char c;
    int j =0;

    //Lire la version
    c = fgetc(input); c = fgetc(input);
    if (c != '2') {
        printf("Mauvais format de PGM\n");
        exit(1);
    }
    c = fgetc(input);

    //Lire la taille
    char size[5];
    for (int i=0;i<5;i++) size[i]='\0';
    while (c!= ' ') {
        size[j] = c;
        j++;
        c = fgetc(input);
    }
    data->nx = atoi(size);
    c = fgetc(input); j = 0;

    char size2[5];
    for (int i=0;i<5;i++) size2[i]='\0';
    while (c!= '\n') {
        size2[j] = c;
        j++;
        c = fgetc(input);
    }
    data->ny = atoi(size2);
    c = fgetc(input); j = 0;

    //Lire le grayscale
    char gray[5];
    for (int i=0;i<5;i++) gray[i]='\0';
    while (c!= '\n') {
        gray[j] = c;
        j++;
        c = fgetc(input);
    }
    data->max_gris = atoi(gray);
    c = fgetc(input); j = 0;

    //Alouer le tableau
    data->t = malloc(sizeof(int*)*data->nx);
    for(int i=0; i<data->nx; i++) data->t[i] = malloc(sizeof(int)*data->ny);

    //Lire les nombres
    int k =0;
    char nn[5];
    for(int i = 0; i < 5; i++) nn[i] = '\0';
    while(c != EOF) {
        while(c != '\n' && c != ' ') {
            nn[k] = c;
            k++;
            c = fgetc(input);
        }
        data->t[j/(data->nx)][j%(data->nx)] = atoi(nn);
        for(int i = 0; i < 5; i++) nn[i] = '\0';
        j++; c = fgetc(input); k =0;
    }
}


//Free une graymap
void freePGM(PGMData* data) {
    if (data == NULL) return;
    for(int i = 0; i < data->nx; i++) free(data->t[i]);
    free(data->t);
    free(data);
}
#ifndef _UTILS_H_
#define _UTILS_H_


#include <stdbool.h>
#include <math.h>



double min(double a, double b);

double map(double x, double xmin, double xmax, double ymin, double ymax);

double clamp(double v, double min, double max);

//Renvoi le nombre de chiffres dans un nombre
int nombre_chiffres(int n);

//Fonction gausienne de densité
double f_gauss(double x, double y);

//Fonction homogène de densité
double f_const(double x, double y);

#endif
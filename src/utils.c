
#include "utils.h"


double min(double a, double b) {
    return (a > b ? b : a);
}

double map(double x, double xmin, double xmax, double ymin, double ymax){
    return (((x - xmin)*(ymax - ymin))/(xmax - xmin)) + ymin;
}

double clamp(double v, double min, double max) {
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

//Renvoi le nombre de chiffres dans un nombre
int nombre_chiffres(int n) {
    return (n==0)?1:log10(n)+1; 
}

//Fonction gausienne de densité
double f_gauss(double x, double y) {
    return exp(-2*x*x - 2*y*y);
}

//Fonction homogène de densité
double f_const(double x, double y) {
    return .5;
}


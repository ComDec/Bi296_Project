#ifndef __NUMBER_UTILS_H
#define __NUMBER_UTILS_H

/**
 * Indica si un numero esta entre a y b.
 * @param num El numero a comprobar.
 * @param a Inicio del intervalo.
 * @param b Fin del intervalo.
 * @return 1 si se cumple que el a <= num < b
 */
int isBetween(double num, double a, double b);

/**
 * Genera un numero aleatorio que cumple 0 <= x < 1
 * @return Numero aleatorio.
 */
double generateRandom();

void sort(double *array, int n);

/**
 * Devuelve un array resultado de convertir decimal a la base. Por ejemplo, si decimal = 50 y base = 4, return [3,0,2]
 * @param decimal El numero decimal que queremos cambiar de base.
 * @param base La base a la que queremos cambiar el numero decimal.
 * @param n Longitud del array de salida, insertando ceros a la izquierda.
 * @return Array de enteros con el decimal en la base especificada.
 */
int* changeBase(int decimal, int base, int n);

#endif

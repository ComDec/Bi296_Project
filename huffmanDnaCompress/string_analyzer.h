#ifndef __STRING_ANALIZER_H
#define __STRING_ANALIZER_H

#include "string_list.h"
#include "string_value_list.h"

#define INITIAL_EVALUATION 0.2
#define BUFFER_LEN 30
#define BLANK_CHAR '_'

/**
 * A partir de un prefijo, amplia dicho prefijo con simbolos del alfabeto, devolviendo
 * los susceptibles de ser agrupados.
 * @param input Cadena de entrada.
 * @param alphabet Alfabeto
 * @param prefix prefijo a partir del cual buscar la palabra optima.
 * @param prefixValue Valor del prefijo.
 * @param expansionList Lista de prefijos encontrados. 
 * @param resLength Aqui se guarda la longitud del resultado. En la primera llamada es 0.
 */
void expandBestCombination(char * input, char *alphabet, char *prefix, double prefixValue, StringValueList* expansionList);

/**
 * Forma palabras con alto grado de repeticion en input
 * @param input Cadena de entrada.
 * @param alphabet cadena con todos los simbolos del alfabeto.
 * @return Array con palabras repetidas
 */
StringValueList* groupSymbols(char * input, char *alphabet);

/**
 * Devuelve todas las combinaciones posibles del alfabeto en n caracteres.
 * Por ejemplo, alfabeto atcg, n = 2, la funcion retornara aa,at,ac,ag,ta,tt,tc,tg...
 * @param alphabet Cadena con los simbolos del alfabeto.
 * @param combinationLength Longitud de las combinaciones que queremos formar.
 * @return Array de char* con las combinaciones creadas.
 */
StringList* getAlphabetCombinations(char *alphabet, int combinationLength);

/**
 * Devuelve un numero decimal que evalua el valor del grupo en la entrada.
 * Es el resultado de dividir el numero de apariciones del grupo entre la longitud de input y multiplicado por la longitud del grupo.
 * @param apparitions numero de veces que aparece la secuencia en la entrada.
 * @param inputLength Longitud de la entrada
 * @param sequenceLength Longitud de la secuencia.
 * @return Numero decimal que indica el valor de la subcadena.
 */
double evaluate(int apparitions, int inputLength, int sequenceLength);



#endif

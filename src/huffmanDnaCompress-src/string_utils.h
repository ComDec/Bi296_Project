#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#define BUFFER_SEARCH_POSITION_LEN 128
#define OVERLAPPED_SEARCH 1
#define BLANK_CHAR '_'
/**
 * Elimina de input las ocurrencias de substring, sustituyendolas por el caracter '_'
 * @param input Cadena de entrada.
 * @param substring Cadena a eliminar de input.
 */
void deleteSubstring(char *input, char *substring);

/**
 * Busca la secuencia sequence dentro de dnaString.
 * @param input La cadena de entrada donde se busca la secuencia.
 * @param substring La secuencia a buscar.
 * @param nFound Aqui se guarda el numero de secuencias encontradas.
 * @param overlappedSearch Indica si al encontrar un substring, el siguiente se sigue buscando cuando termina
 * 	el anterior o en el siguiente caracter.
 * @return Devuelve una lista de enteros indicando la posicion donde aparece la secuencia en la cadena de dna.
 */
int* searchSubstring(char* input, char* substring, int *nFound, int overlappedSearch);

/**
 * Cuenta los caracteres de input exceptuando BLANK_CHAR.
 * @param input Entrada
 * @return El numero de caracteres validos.
 */
int countValidCharacters(char *input);

/**
 * Convierte un char en un string
 * @param c El char a convertir
 * @return char*
 */
char* createString(char c);

/**
 * Devuelve el substring de input que va desde start(inclusive) hasta end(inclusive).
 */
char* substring(char* input, int start, int end);

/**
 * Crea una nueva cadena resultado de concatenar dest con src.
 */
char* concat(char *a, char *b);

/**
 * Devuelve un puntero de doubles con la frecuencia de cada simbolo del alfabeto en input.
 */
double* charFreq(char *input, char *alphabet);

/**
 * Devuelve un puntero de int con el conteo de cada simbolo del alfabeto en input.
 */
int* charCount(char *input, char *alphabet);
#endif

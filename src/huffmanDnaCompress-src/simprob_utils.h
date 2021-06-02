#ifndef __SIMPROB_UTILS_H
#define __SIMPROB_UTILS_H

#define ADENINE 'A'
#define GUANINE 'G'
#define CYTOSINE 'C'
#define THYMINE 'T'

/**
 * Crea una cadena de DNA en base a los parametros
 * @param adeninePercentage Porcentaje de bases adenina.
 * @param guaninePercentage Porcentaje de bases guanina.
 * @param cytosinePercentage Porcentaje de bases citosina.
 * @param thyminePercentage Porcentaje de bases timina.
 * @param dnaLength Longitud de la cadena, dada en pb.
 * @return Cadena de adn con las caracteristicas necesarias.
 */
char* createDnaString(double adeninePercentage, double guaninePercentage, double cytosinePercentage, double thyminePercentage, int dnaLength);

/**
 * Obtiene un nucleotido (A,T,C,G) en base a los porcentajes pasados por parametro.
 * @param adeninePercentage Porcentaje de adenina.
 * @param guaninePercentage Porcentaje de guanina.
 * @param cytosinePercentage Porcentaje de citosina.
 * @param thyminePercentage Porcentaje de timina.
 * @return Un nucleotido (A,T,C,G) en base a los porcentajes pasados por parametro.
 */
char createNucleotide(double adeninePercentage, double guaninePercentage, double cytosinePercentage, double thyminePercentage);

#endif

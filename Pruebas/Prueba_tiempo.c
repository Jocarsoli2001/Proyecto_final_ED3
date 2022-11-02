/*
	Prueba_tiempo.c
	02 nov 22, Guatemala, Guatemala
	Universidad del Valle, Electrónica Digital 3
	Por: Alejandro Ramirez y José Carlo Santizo
	
*/
/*
 +--------------------------------------------------------------------+
 |                              LIBRERIAS                             |
 +--------------------------------------------------------------------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>		// Cadena

#include <sys/time.h>	// Tiempo
#include <time.h>

/*
 +--------------------------------------------------------------------+
 |                         VARIABLES GLOBALES                         |
 +--------------------------------------------------------------------+
*/  
// Variables respecto a tiempo
static int seconds_last = 99;
char TimeString[128];

struct timeval current_time;
double delta, total, tiempo_inicial, tiempo_actual;

/* 	
 +--------------------------------------------------------------------+
 |                                MAIN                                |
 +--------------------------------------------------------------------+
*/ 
int main(void)
{
	// Iniciar función
	gettimeofday(&current_time, NULL);

	// Obtener tiempo inicial
	tiempo_inicial = current_time.tv_sec + current_time.tv_usec/1000000.0;

	while(1)
	{
		// Obtener valores de gettimeofday()
		gettimeofday(&current_time, NULL);

		// Generar tiempo total
		tiempo_actual = current_time.tv_sec + current_time.tv_usec/1000000.0;
		//printf("Segundos : %ld,  Micro segundos : %ld, Tiempo actual: %lf\n", current_time.tv_sec, current_time.tv_usec, tiempo_actual);

		// Generar un delta
		delta = (tiempo_actual - tiempo_inicial);
		printf("Tiempo delta: %f\n", delta);
		
	}
	return 0;
}





	
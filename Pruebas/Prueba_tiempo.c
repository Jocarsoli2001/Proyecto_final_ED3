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

/*
 +--------------------------------------------------------------------+
 |                         VARIABLES GLOBALES                         |
 +--------------------------------------------------------------------+
*/  
	// Variables respecto a tiempo
	static int seconds_last = 99;
	char TimeString[128];
	timeval curTime;
/* 	
 +--------------------------------------------------------------------+
 |                                MAIN                                |
 +--------------------------------------------------------------------+
*/ 
int main(void)
{

	gettimeofday(&curTime, NULL);
	if (seconds_last == curTime.tv_sec)
		return;
	
	seconds_last = curTime.tv_sec;
	
	strftime(TimeString, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
	
	return 0;
}





	
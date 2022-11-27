


/*
 +--------------------------------------------------------------------+
 |                              LIBRERIAS                             |
 +--------------------------------------------------------------------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>		// Cadena
#include <wiringPiSPI.h> // GPIOS libreria
#include <pthread.h>		// Uso de hilos

/*
 +--------------------------------------------------------------------+
 |                         VARIABLES GLOBALES                         |
 +--------------------------------------------------------------------+
*/
#define SPI_CHANNEL 0 // Canal 0 del SPI de la raspy
#define SPI_CLOCK_SPEED 1000000	// 1MHz del SPISCLk

int contSPI    = 0;
int val0, val1, val2 = 0;
int F_A_SPI = 0;
int R_SPI   = 0;
int E_Alarma = 0;
float V_R_SPI = 0;

/*
 +--------------------------------------------------------------------+
 |                      PROTOTIPOS DE FUNCIONES                       |
 +--------------------------------------------------------------------+
*/ 
void Bocina(void);
/*
 +--------------------------------------------------------------------+
 |                                MAIN                                |
 +--------------------------------------------------------------------+
*/ 
int main(int argc, char **argv) // Acepta argumentos el feo
{	
	printf(" Master_SPI_con_Arduino\n");
	
	pthread_t thread1;
	pthread_create(&thread1, NULL, (void*)&Bocina, NULL);
	
	while(1){
		/* 
		   Devuelve un File Descriptor, en caso contrario hay error.
		   Existe wiringPISetup, pero no te permite definir el tipo 
		   de modo SPI, en este caso es 0 para coincidir con el modo
		   estandar de SPI del arduino.
		*/
		int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
		if (fd == -1) { // Verificar si no se creo el fd
			printf("Failed to init SPI communication.\n");
			return -1;
		}
		// Avisa que se ha establecido la comunicacion
		//printf("SPI communication successfully setup.\n");
		/*
		   Enviar valores, es raro. Solo puedes enviar un array. En el 
		   parametro de la funcion defines el canal de origen, la variable 
		   tipo array y el tamaño del array.
			
		   Al ser SPI, la comunicación es bidireccional y por lo tanto, 
		   al enviar un mensaje se recibe uno. Por lo tanto, primero envia
		   el primer valor del arreglo la raspy y luego recibe un valor
		   basura dado por el arduino. Al momento de enviar un valor basura 
		   desde la raspy, que es el segundo valor de arreglo, recibimos el
		   valor que originalmente nos envia el arduino.
		 
		*/
		unsigned char buf[2] = { 3, 0 };
		wiringPiSPIDataRW(SPI_CHANNEL, buf, 2);
		contSPI++;
		
		switch(contSPI){
			case 1:
				val0 = buf[1];
				usleep(1000);
				break;
			case 2:
				val1 = buf[1];
				usleep(1000);
				break;
			case 3:
				val2 = buf[1];
				usleep(1000);
				contSPI = 0;
				F_A_SPI = 1;
				break;
			default:
				contSPI = 0;
				break;
		}
		
		if(val0 == val1 && val1 == val2 && F_A_SPI){
			R_SPI = val0;
			F_A_SPI = 0;
			//printf("   V1:%d V2:%d V3:%d \n",val0,val1,val2); 
			usleep(10000);

		}
		V_R_SPI = (float)R_SPI/51; // 5/255 = 1/51
		//printf("Voltaje: %.2f \n", V_R_SPI);
		
		if(V_R_SPI > 2.5 || V_R_SPI < 0.5){
			E_Alarma = 1;
		}else{
			E_Alarma = 0;
			//printf("Parametros normales.");
		}
		
		close(fd); // Cerrar los fd abiertos para la recepción
	}
	pthread_join(thread1, NULL);
}

/*
 +--------------------------------------------------------------------+
 |                              FUNCIONES                             |
 +--------------------------------------------------------------------+
*/ 

void Bocina(void){
	while(1){
		if(E_Alarma == 1){
			printf("Fuera de rango.");
			printf("V:%.2f \n", V_R_SPI);
			//printf("Aqui\n");	
		}
		else if(E_Alarma == 0){
			printf("Parametros normales.");
			printf("V: %.2f\n", V_R_SPI);
		}
		//usleep(1000000);
	}
	pthread_exit(0);
}


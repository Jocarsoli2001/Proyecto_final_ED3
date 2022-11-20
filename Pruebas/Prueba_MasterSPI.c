#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>		// Cadena
#include <wiringPiSPI.h> // GPIOS libreria

#define SPI_CHANNEL 0 // Canal 0 del SPI de la raspy
#define SPI_CLOCK_SPEED 1000000	// 1MHz del SPISCLk

int main(int argc, char **argv) // Acepta argumentos el feo
{
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
    printf("Data returned: %d \n",buf[1]); 
	close(fd);
	usleep(100000);
}
}

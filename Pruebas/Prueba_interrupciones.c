/*

José Carlo Santizo Olivet - Carné 20185
Consumidor

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <wiringPi.h>

//------------------------------------------ Definiciones ------------------------------------------
#define BUTTON_PIN 27
#define LED1 12
#define LED2 25

//----------------------------- Declaraciones de variables globales --------------------------------
// Definición para generar FIFOs
int dummy;
int tuboi;		                                        // Para los file descriptors
int Num_M = 0;
int Num_N = 0;

// Variables para generar array de chars
char StringArray[MAX_CADENAS][MAX_LETRAS];
char Buffer[MAX_LETRAS];                                        // Buffer común entre todos los hilos
char cmd[50];
char Tubos[50];
int contador1 = 1, contador2 = 1;
char Hilo_num[20];
char Tubo_hilo[M_max];
volatile int eventCounter = 0; 


// Variable para Semaphore
sem_t my_semaphore;

//------------------------------------ Prototipo de funciones --------------------------------------
void hilo1(void *ptr);
void hilo2(void *ptr);
void Funcion(void *ptr);
void impresion_final();

//-------------------------------------------- Main ------------------------------------------------
int main(void) {

    // Setup de WiringPi
	wiringPiSetupGpio();
	wiringPiSetupSys();

    // set Pin 17/0 generate an interrupt on high-to-low transitions
    // and attach myInterrupt() to the interrupt
    if (wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, &myInterrupt) < 0 ) 
    {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }

    // Declaración de pines
    pinMode(BUTTON_PIN, INPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    // display counter value every second.
    while ( 1 ) {
        
        // Prender LED1
        digitalWrite(LED1, HIGH);

        // Sleep de un 1 segundo
        sleep(1);

        // Prender LED2 y apagar LED1
        digitalWrite(LED1, LOW);

        // Sleep de un segundo
        sleep(1);

    }

    return 0;
}

//----------------------------------------- Funciones ----------------------------------------------
void myInterrupt(void) {
   
   //

}



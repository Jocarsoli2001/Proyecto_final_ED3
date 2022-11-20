/*

Alejandro Ramirez Morales - Carne 20388

José Carlo Santizo Olivet - Carne 20185

Prueba interrupciones



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

#define Push1 17

#define Push2 27

#define Inte1 22

#define Inte2 5



#define LED1 23



#define LEDR 24

#define LEDG 25

#define LEDB 12

//----------------------------- Declaraciones de variables globales --------------------------------

int cont  = 0;

int flag1 = 0;



// Variable para Semaphore

sem_t my_semaphore;



//------------------------------------ Prototipo de funciones --------------------------------------

void hilo1(void *ptr);

void hilo2(void *ptr);

void Funcion(void *ptr);

void impresion_final();

void myInterrupt(void);







//-------------------------------------------- Main ------------------------------------------------

int main(void) {



    // Setup de WiringPi

	wiringPiSetupGpio();

	wiringPiSetupSys();



    // set Pin 17/0 generate an interrupt on high-to-low transitions

    // and attach myInterrupt() to the interrupt

    if (wiringPiISR (Push1, INT_EDGE_FALLING, &myInterrupt) < 0 ) 

    {

        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));

        return 1;

    }



    // Declaración de pines

    pinMode(Push1, INPUT);

	pinMode(Push2, INPUT);

	pinMode(Inte1, INPUT);

	pinMode(Inte2, INPUT);

	

	pinMode(LED1, OUTPUT);

	

    pinMode(LEDR, OUTPUT);

    pinMode(LEDG, OUTPUT);

	pinMode(LEDB, OUTPUT);

	

	

    // display counter value every second.

    while ( 1 ) {

		cont++;

        printf("Cont val: %d\n", cont);

        // Sleep de un segundo

        usleep(100000);



    }



    return 0;

}



//----------------------------------------- Funciones ----------------------------------------------

void myInterrupt(void) {

	

	if(digitalRead(Push1)==0){

		flag1 = 1;

	}

	

	if(digitalRead(Push1)==0 && flag1 == 1 ){

		flag1 = 0;

        // Para prender LEDs cuando exista una interrupción

        digitalWrite(LED1, HIGH);

        usleep(50000);

        digitalWrite(LED1, LOW);

        usleep(1);

	}

}






/*
	Prueba_interrupciones_Completa.c
	19 nov 22, Guatemala, Guatemala
	Universidad del Valle, Electrónica Digital 3
	Por: Alejandro Ramirez - 20388, Jose Santizo - 20185
	
	
	Instrucciones: El programa es capaz de detectar los cambios de 
    estado en los switches y en los botones sin rebote. Consta de 
    un hilo que compara el estado de las entradas y las compara con 
    banderas activadas por las interrupciones de wiringPi de los gpios.
    
    TODAS LAS ENTRADAS  TIENEN CONFIGURACION PULL-DOWN. 
    
    Los botones cambian el estados de los relays del sistema, y 
    los interruptores despliegan en la terminal cuando sucede un cambio
    de estado en los switches.
    
    Si se busca cambiar lo que realiza cada entrada, se debe ir al hilo
    dedicado a ellas llamado Lec_PeI.   
    
    
*/
/*
 +--------------------------------------------------------------------+
 |                              LIBRERIAS                             |
 +--------------------------------------------------------------------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>	// Uso de hilos
#include <string.h>     // Cadenas
#include <wiringPi.h>   // Int de gpios, gpios como arduino
/*
 +--------------------------------------------------------------------+
 |                         VARIABLES GLOBALES                         |
 +--------------------------------------------------------------------+
*/
// Se asocia una variable a un GPIO de la raspy.
#define PUSH1 12
#define PUSH2 21
#define INTE1 20
#define INTE2 16
#define RELAY1 26
#define RELAY2 19

// Banderas de interrupcion P, Botones; I, Interruptores.
int F_P  = 0;
int F_P2 = 0;
int F_I  = 0;
int F_I2 = 0;
// Estados de interrupcions propios de switches, almacenan estados 
// iniciales o anteriores.
int E_I  = 0;
int E_I2 = 0;

// Variable basura, para verificar que no se registran mas pulsaciones
// de las esperadas.
int eventCounter  = 0;
int eventCounter2 = 0;

/*
 +--------------------------------------------------------------------+
 |                      PROTOTIPOS DE FUNCIONES                       |
 +--------------------------------------------------------------------+
*/ 
void myInterrupt(void);
void Lec_PeI(void);

/*
 +--------------------------------------------------------------------+
 |                                MAIN                                |
 +--------------------------------------------------------------------+
*/ 
int main(void) {
    // sets up the wiringPi library
    if (wiringPiSetupGpio () < 0) {
      printf ("Unable to setup wiringPi: %s\n");
      return 1;
    }
    
    // Configura las interrupciones de gpios
    if ( wiringPiISR (PUSH1, INT_EDGE_RISING, &myInterrupt) < 0 ) {
      printf ("Unable to setup ISR: %s\n");
      return 1;
    }
    if ( wiringPiISR (PUSH2, INT_EDGE_RISING, &myInterrupt) < 0 ) {
      printf ("Unable to setup ISR: %s\n");
      return 1;
    }
    if ( wiringPiISR (INTE1, INT_EDGE_BOTH, &myInterrupt) < 0 ) {
      printf ("Unable to setup ISR: %s\n");
      return 1;
    }
    if ( wiringPiISR (INTE2, INT_EDGE_BOTH, &myInterrupt) < 0 ) {
      printf ("Unable to setup ISR: %s\n");
      return 1;
    }
    
    // Configuracion de los gpios
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(PUSH1, INPUT);
    pinMode(PUSH2, INPUT);
    pinMode(INTE1, INPUT);
    pinMode(INTE2, INPUT);

    // Valor de los estados de interrupcion
    E_I  = digitalRead(INTE1);
    E_I2 = digitalRead(INTE2);

    // Creacion de los hilos
    pthread_t thread1;
    pthread_create(&thread1, NULL, (void*)&Lec_PeI, NULL);
    
    // display counter value every second.
    while ( 1 ) {
        
    }
    pthread_join(thread1, NULL);
    return 0;
}

/*
 +--------------------------------------------------------------------+
 |                              FUNCIONES                             |
 +--------------------------------------------------------------------+
*/ 
void Lec_PeI(void){
    while(1){
        if(digitalRead(PUSH1) == 1 && F_P == 1){
            // Accion a ejecutar con el botón 1
            digitalWrite(RELAY1, LOW);
            usleep(50000);
            digitalWrite(RELAY1, HIGH);
            usleep(50000);
            printf("Interrupcion1: %d\n",eventCounter);
            eventCounter++;
            F_P = 0;
        }
        else if(digitalRead(PUSH2) == 1 && F_P2 == 1){
            // Accion a ejecutar con el botón 2
            digitalWrite(RELAY2, LOW);
            usleep(50000);
            digitalWrite(RELAY2, HIGH);
            usleep(50000);
            printf("Interrupcion2: %d\n",eventCounter2);
            eventCounter2++;
            F_P2 = 0;
        }
        if(F_I == 1){
            // Accion a ejecutar con el interruptor 1
            F_I = 0;
            printf("1 Hubo un cambio, rey.\n");
        }
        if(F_I2 == 1){
            // Accion a ejecutar con el interruptor 2
            F_I2 = 0;
            printf("2 Hubo un cambio, rey.\n");
        }
        usleep(10000);
    }
    pthread_exit(0);
}

void myInterrupt(void) {
    // Banderazo para botones
/*
 * El antirrebote para los botones solamente utiliza software,
 * aplicando las banderas para que solamente opere un boton y que se 
 * leea un cambio de estado ideal. 
 * La logica es que cuando se de un cambio de estado creciente,
 * se levante una bandera, se ejecute la funcion del boton(Desde un 
 * hilo) al ser presionado y la bandera caiga. Para que una pulsacion 
 * sea una ejecucion. 
*/
    if(digitalRead(PUSH1) == 1 && digitalRead(PUSH2) == 0 && 
       digitalRead(INTE1) == E_I && digitalRead(INTE2) == E_I2){
        F_P =1;
        F_P2=0;
    }
    if(digitalRead(PUSH2) == 1 && digitalRead(PUSH1) == 0 && 
       digitalRead(INTE1) == E_I && digitalRead(INTE2) == E_I2){
        F_P2=1;
        F_P =0;
    }
    
    // Banderazo para interruptores
/*
 * Para la detección del cambio de estado, se guardo el estado inicial
 * de cada interruptor. Luego en caso de un cambio en alguno de los ejes
 * del interruptor, se compara con su valor inicial, en caso haya 
 * cambiado, se actualiza el valor a comparar en el futuro y 
 * se activa una bandera para ejecutar la respectiva acción, 
 * donde en el hilo se apaga la vandera.
 */
    if(digitalRead(INTE1) != E_I){
        F_I = 1;
        E_I = !E_I;
    }
    if(digitalRead(INTE2) != E_I2){
        F_I2 = 1;
        E_I2 = !E_I2;
    }
    
    usleep(10000);
}

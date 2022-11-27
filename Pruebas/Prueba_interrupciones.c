/*
 +--------------------------------------------------------------------+
 |                              LIBRERIAS                             |
 +--------------------------------------------------------------------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
/*
 +--------------------------------------------------------------------+
 |                         VARIABLES GLOBALES                         |
 +--------------------------------------------------------------------+
*/ 
// Use GPIO Pin 17, which is Pin 0 for wiringPi library
int F_P = 0;
#define BUTTON_PIN 12
#define LED 26

// the event counter 
int eventCounter = 0;

// -------------------------------------------------------------------------
/*
 +--------------------------------------------------------------------+
 |                      PROTOTIPOS DE FUNCIONES                       |
 +--------------------------------------------------------------------+
*/
void myInterrupt(void) {

    
    if(digitalRead(BUTTON_PIN) == 1){
        F_P=1;
    }
    /*
    if(digitalRead(BUTTON_PIN) == 0 && F_P == 1){
        digitalWrite(LED, LOW);
        usleep(500000);
        digitalWrite(LED, HIGH);
        usleep(500000);
        printf("Interrupcion\n");
        eventCounter++;
         F_P=1;
    }
    */

}

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
    
    pinMode(LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    // set Pin 17/0 generate an interrupt on high-to-low transitions
    // and attach myInterrupt() to the interrupt
    if ( wiringPiISR (BUTTON_PIN, INT_EDGE_RISING, &myInterrupt) < 0 ) {
      printf ("Unable to setup ISR: %s\n");
      return 1;
    }

    // display counter value every second.
    while ( 1 ) {
        if(digitalRead(BUTTON_PIN) == 1 && F_P == 1){
            digitalWrite(LED, LOW);
            usleep(50000);
            digitalWrite(LED, HIGH);
            usleep(50000);
            printf("Interrupcion: %d\n",eventCounter);
            eventCounter++;
            F_P = 0;
        }
    }

    return 0;
}
/*
 +--------------------------------------------------------------------+
 |                              LIBRERIAS                             |
 +--------------------------------------------------------------------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <wiringPi.h>          
#include <lcd.h>               
 
/*
 +--------------------------------------------------------------------+
 |                         VARIABLES GLOBALES                         |
 +--------------------------------------------------------------------+
*/  
//USE WIRINGPI PIN NUMBERS
#define LCD_RS  24               //Register select pin
#define LCD_E   23               //Enable Pin
#define LCD_D4  18               //Data pin 4
#define LCD_D5   1               //Data pin 5
#define LCD_D6   7              //Data pin 6
#define LCD_D7  25               //Data pin 7
 
/* 	
 +--------------------------------------------------------------------+
 |                                MAIN                                |
 +--------------------------------------------------------------------+
*/ 

int main()
{
    int lcd;               
    wiringPiSetupGpio();    
    wiringPiSetupSys();    
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    lcdPosition(lcd, 0, 0); 
    lcdPuts(lcd,"Hola, no soy"); 
    lcdPosition(lcd, 0, 1);
    lcdPuts(lcd,"calculadora");
    sleep(10);
}

/*
	Código para ejecutar en las Unidades Terminales Remotas (UTR)
	19 nov 22, Guatemala, Guatemala
	Universidad del Valle, Electrónica Digital 3
	Por: Alejandro Ramirez - 20388, Jose Santizo - 20185
    
    TODAS LAS ENTRADAS  TIENEN CONFIGURACION PULL-DOWN.  
    // Compilar con -lwiringPi -lwiringPiDev -lpthread   
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


#include <pthread.h>		// Uso de hilos
#include <string.h>     	// Cadenas
#include <wiringPi.h>   	// Int de gpios, gpios como arduino
#include <lcd.h>   			// LCD
#include <wiringPiSPI.h> 	// SPI
#include <sys/time.h>	// Tiempo

// LIBRERIAS DE RED

#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>
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
#define INTE3 6
#define RELAY1 26
#define RELAY2 19
#define RELAY3 13

// Banderas de interrupcion P, Botones; I, Interruptores.
int F_P  = 0;
int F_P2 = 0;
int F_I  = 0;
int F_I2 = 0;
int F_I3 = 0;
// Estados de interrupcions propios de switches, almacenan estados 
// iniciales o anteriores.
int E_I  = 0;
int E_I2 = 0;
int E_I3 = 0;

int APush = 0;
int BPush = 0;

int AB1 = 0;
int AB2 = 0;
int AB3 = 0;

// Pines de la lcd
#define LCD_RS  24               //Register select pin
#define LCD_E   23               //Enable Pin
#define LCD_D4  18               //Data pin 4
#define LCD_D5   1               //Data pin 5
#define LCD_D6   7              //Data pin 6
#define LCD_D7  25               //Data pin 7

// Cadenas de la lcd
char Linea1[]  = "";
char Linea2[]  = "";
char Voltaje[] = "";

int lcd;     
int E_LCD = 0;          

// Variable basura, para verificar que no se registran mas pulsaciones
// de las esperadas.
int eventCounter  = 0;
int eventCounter2 = 0;

// Variables de SPI
#define SPI_CHANNEL 0 // Canal 0 del SPI de la raspy
#define SPI_CLOCK_SPEED 1000000	// 1MHz del SPISCLk

int contSPI    = 0;
int val0, val1, val2 = 0;
int F_A_SPI = 0;
int R_SPI   = 0;
float x = 0;
// Alarma
int E_Alarma = 0;
int Tipo_Alarma = 0;
float Sobre_V = 3;
float Bajo_V = 1.5;
#define BUZZER 5
#define LED_A   22
#define LED_R1  27
#define LED_R2   4
#define MSG_SIZE 200                                                                       // Tamaño máximo del mensaje

// Luz
int F_Luz =0;
int F_Luz2 =0;

#define In_ESP 17

// Variables respecto a tiempo
static int seconds_last = 99;
char TimeString[128];

struct timeval current_time;
double delta, total, tiempo_inicial, tiempo_actual;

char HoyEs[100] = "";

// SOCKETS
int sockfd, n;
unsigned int length;
struct sockaddr_in server, addr;
char buffer_main[MSG_SIZE];	                                                                    // Para guardar mensajes recibidos o para mandar
int boolval = 1;			                                                                // Utilizado en una opción del socket
char Broadcast[] = "192.168.43.255";
char MegaString[MSG_SIZE];

// Banderazo para eventos
int Ev_Estado0 = 0;
int Ev_B1 = 0;
int Ev_B2 = 0;
int Ev_I1 = 0;
int Ev_I2 = 0;
int Ev_I3 = 0;
int Ev_Vo = 0;
int Ev_Hi = 0;
int Ev_IOT = 0;

/*
 +--------------------------------------------------------------------+
 |                      PROTOTIPOS DE FUNCIONES                       |
 +--------------------------------------------------------------------+
*/ 
void myInterrupt(void);
void Lec_PeI(void);
void LCD_P(void);
void A_Estados(void);
void ADC_SPI(void);
void Bocina(void);
void Luz(void);
void Tiempo(void);
void Luz2(void);

/*
 +--------------------------------------------------------------------+
 |                                MAIN                                |
 +--------------------------------------------------------------------+
*/ 
int main(void) {


	sockfd = socket(AF_INET, SOCK_DGRAM, 0);                                                // Crea el socket en configuración "Connectionless"
	if(sockfd < 0)
		error("Opening socket");

	server.sin_family = AF_INET;		                                                    // Para el dominio de internet
	server.sin_port = htons(4020);		                                            		// Número de puerto
	server.sin_addr.s_addr = htonl(INADDR_ANY);	                                            // Para recibir de cualquier interfaz de red

	length = sizeof(server);			                                                    // Tamaño de la estructura

	// binds the socket to the address of the host and the port number
	if(bind(sockfd, (struct sockaddr *)&server, length) < 0){
		error("Error binding socket.");
    }

	// change socket permissions to allow broadcast
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0){
   		error("Error setting socket options\n");
    }

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
    if ( wiringPiISR (INTE3, INT_EDGE_BOTH, &myInterrupt) < 0 ) {
      printf ("Unable to setup ISR: %s\n");
      return 1;
    }
      
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    
    // Configuracion de los gpios
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(LED_A, OUTPUT);
    pinMode(LED_R1, OUTPUT);
    pinMode(LED_R2, OUTPUT);
    pinMode(In_ESP, OUTPUT);
    pinMode(PUSH1, INPUT);
    pinMode(PUSH2, INPUT);
    pinMode(INTE1, INPUT);
    pinMode(INTE2, INPUT);
    pinMode(INTE3, INPUT);
    
    // Valor de los estados de interrupcion
    E_I  = digitalRead(INTE1);
    E_I2 = digitalRead(INTE2);
    E_I3 = digitalRead(INTE3);
	
	// Iniciar función de tiempo
	gettimeofday(&current_time, NULL);

	// Obtener tiempo inicial
	tiempo_inicial = current_time.tv_sec + current_time.tv_usec/1000000.0;
    
    // Creacion de los hilos
    pthread_t thread1,thread2,thread3, thread4, thread5, thread6,
			  thread7,thread8;
    pthread_create(&thread1, NULL, (void*)&Lec_PeI, NULL);
    pthread_create(&thread2, NULL, (void*)&LCD_P, NULL);
    pthread_create(&thread3, NULL, (void*)&A_Estados, NULL);
	pthread_create(&thread4, NULL, (void*)&ADC_SPI, NULL);
	pthread_create(&thread5, NULL, (void*)&Bocina, NULL);
	pthread_create(&thread6, NULL, (void*)&Luz, NULL);
	pthread_create(&thread7, NULL, (void*)&Tiempo, NULL);
	pthread_create(&thread8, NULL, (void*)&Luz2, NULL);
    // display counter value every second.
    
    printf(" UTR 1\n");
    // HILO DE ENVIO
    while ( 1 ) {

		// Obtener valores de gettimeofday()
		gettimeofday(&current_time, NULL);

		// Generar tiempo total
		tiempo_actual = current_time.tv_sec + current_time.tv_usec/1000000.0;
		//printf("Segundos : %ld,  Micro segundos : %ld, Tiempo actual: %lf\n", current_time.tv_sec, current_time.tv_usec, tiempo_actual);

		// Generar un delta
		delta = (tiempo_actual - tiempo_inicial);

		memset(MegaString, 0, MSG_SIZE);										// Setea todos los valores a cero
		memset(buffer_main, 0, MSG_SIZE);										// Setea todos los valores a cero
		
		// MegaString es vital para la transferencia de estados con el historiador
		sprintf(MegaString," #1,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%d,%d,%d,%.2f",APush,BPush,AB1,AB2,AB3,!AB1,!AB2,!AB3,x,E_Alarma,F_Luz,F_Luz2,tiempo_actual); // Concatenación     
		printf(" %s\n",MegaString);      
		
		// Recibe de un cliente
		n = recvfrom(sockfd, buffer_main, MSG_SIZE, 0, (struct sockaddr *)&addr, &length);

	    if(n < 0){
	 		error("recvfrom"); 
        }

		if(strcmp(buffer_main,"Prender Led 1") == 0){
			F_Luz = 1;
			printf("La luz que controla está en estado: %d",F_Luz);
		}
		if(strcmp(buffer_main,"Apagar Led 1") == 0){
			F_Luz = 0;
			printf("La luz que controla está en estado: %d",F_Luz);
		}

	    //printf("Se recibició el mensaje: %s\n", buffer_main);

		if((int)(delta) % 2 == 0){
			n = sendto(sockfd, MegaString, sizeof(MegaString), 0,(struct sockaddr *)&addr, length);

			if(n < 0)
				error("sendto");
		}		
	    delay(2000);
    }
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
    pthread_join(thread6, NULL);
	pthread_join(thread7, NULL);
	pthread_join(thread8, NULL);
    return 0;
}

/*
 +--------------------------------------------------------------------+
 |                              FUNCIONES                             |
 +--------------------------------------------------------------------+
*/ 
void Tiempo(void){
	while(1){
		struct timeval tv;
		time_t t;
		struct tm *info;
		char buffer[64];
 
		gettimeofday(&tv, NULL);
		t = tv.tv_sec;

		info = localtime(&t);
		
		sprintf(HoyEs,"%s",asctime (info)); // Concatenacion
		//strftime (HoyEs, sizeof HoyEs, " %I:%M %p.\n", info);
		//printf("%s\n",HoyEs);
		usleep(10000);
	}
	pthread_exit(0);
	
}
void Luz(void){
	while(1){
		if(F_Luz){
			digitalWrite(LED_R1, HIGH);
		}else{
			digitalWrite(LED_R1, LOW);
		}
	}
	pthread_exit(0);
}
void Luz2(void){
	while(1){
		F_Luz2 = digitalRead(In_ESP);
		if(F_Luz2){
			digitalWrite(LED_R2, HIGH);
		}else{
			digitalWrite(LED_R2, LOW);
		}
	}
	pthread_exit(0);
}
void Bocina(void){
	while(1){
		
		if(E_Alarma == 0){
			digitalWrite(BUZZER, LOW);	
			digitalWrite(LED_A, LOW);
			Tipo_Alarma = 1;	
			//printf("Aqui\n");	
		}
		else if(E_Alarma == 1){
			if(BPush == 0){
				digitalWrite(BUZZER, HIGH);
				digitalWrite(LED_A, HIGH);
			}else if(BPush == 1){
				if(Tipo_Alarma){
					digitalWrite(LED_A, HIGH);
					digitalWrite(BUZZER, HIGH);
					usleep(500000);
					digitalWrite(BUZZER, LOW);
					Tipo_Alarma = 0;
				}	
			}
		}
	

	}
	pthread_exit(0);
}

void ADC_SPI(void){
	while(1){
		 // SPI ADC
		int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
		if (fd == -1) { // Verificar si no se creo el fd
			printf("Failed to init SPI communication.\n");
		}
		
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
			//printf("V1:%d V2:%d V3:%d \n",val0,val1,val2); 
			usleep(10000);
		}
		if(x>=Bajo_V && x <= Sobre_V){
			E_Alarma = 0;
			//digitalWrite(LED_R1,LOW); 
			//printf("Alarma: %d\n", E_Alarma);
		}else{
			E_Alarma = 1;
			//digitalWrite(LED_R2, HIGH);
			//printf("Alarma: %d\n", E_Alarma);
		}
		
		close(fd);
	}
	pthread_exit(0);
}

void A_Estados(void){
	while(1){
		if(AB1 == 1){
			digitalWrite(RELAY1, LOW);
		}else{
			digitalWrite(RELAY1, HIGH);
		}
		
		if(AB2 == 1){
			digitalWrite(RELAY2, LOW);
			
		}else{
			digitalWrite(RELAY2, HIGH);
		}
		if(AB3 == 1){
			digitalWrite(RELAY3, LOW);
		}else{
			digitalWrite(RELAY3, HIGH);
		}
		
	}
	pthread_exit(0);
}
void LCD_P(void){
	while(1){
		//lcdClear(lcd); 
		sprintf(Linea1,"R1:%d R2:%d",AB1,AB2); // Concatenacion
		lcdPosition(lcd, 1, 0); 
		lcdPuts(lcd,Linea1); 
		sprintf(Linea1,"R3:%d",AB3); // Concatenacion
		lcdPosition(lcd, 11, 0); 
		lcdPuts(lcd,Linea1); 
		lcdCursor(lcd, 0);
		char Vol [5] ="V:";
		char buf [5] ="";
		char dim [5] = "v";
		char DIM [5] = "V";
		char alr [2] = "";
		x = (float)R_SPI/51;//255;
		//x = (float)x/255;
		float xx = (float)x*0.707;//255;
		//xx = (float)x/255;
		if(APush == 0){
			if(x >1){
				gcvt(x,3,buf);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"v"); 
				lcdCursor(lcd, 0);
				
			}else if( x<1 && x>0.1){
				gcvt(x,2,buf);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"v"); 
				lcdCursor(lcd, 0);
				
			}else if(x<0.1 && x>0){
				gcvt(x,1,buf);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"v"); 
				lcdCursor(lcd, 0);
				
			}else if(x == 0){
				char cero[5] = "0.00";
				strcpy(buf,cero);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"v"); 
				lcdCursor(lcd, 0);
				
			}
		}else{
			if(xx >1){
				gcvt(xx,3,buf);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"V"); 
				lcdCursor(lcd, 0);
				
			}else if( xx<1 && xx>0.1){
				gcvt(x,2,buf);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"V"); 
				lcdCursor(lcd, 0);
				
			}else if(xx<0.1 && xx>0){
				gcvt(x,1,buf);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"V"); 
				lcdCursor(lcd, 0);
				
			}else if(xx == 0){
				char cero[5] = "0.00";
				strcpy(buf,cero);
				strcat(Vol,buf);
				//printf("%s \n",Vol);
				lcdPosition(lcd, 0, 1); 
				lcdPuts(lcd,Vol); 
				lcdPosition(lcd, 6, 1); 
				lcdPuts(lcd,"V"); 
				lcdCursor(lcd, 0);
				
			}
		}

		lcdPosition(lcd, 8, 1); 
		lcdPuts(lcd,"Alarma:"); 
		sprintf(alr,"%d",E_Alarma); // Concatenacion
		lcdPosition(lcd, 15, 1); 
		lcdPuts(lcd,alr); 
		lcdCursor(lcd, 0);
		usleep(20000);
	
		
	}
	pthread_exit(0);
}

void Lec_PeI(void){
    while(1){
        if(digitalRead(PUSH1) == 1 && F_P == 1){
            // Accion a ejecutar con el botón 1
            //AB1 = !AB1;
			
			APush = !APush;
            
            //printf("Interrupcion1: %d\n",eventCounter);
            eventCounter++;
            F_P = 0;
        }
        else if(digitalRead(PUSH2) == 1 && F_P2 == 1){
            // Accion a ejecutar con el botón 2
            //AB2 = !AB2;
            //digitalWrite(RELAY2, LOW);
            //usleep(50000);
            //digitalWrite(RELAY2, HIGH);
            //usleep(50000);
            BPush = !BPush;
            //printf("Interrupcion2: %d\n",eventCounter2);
            eventCounter2++;
            F_P2 = 0;
        }
        if(F_I == 1){
            // Accion a ejecutar con el interruptor 1
            AB1 = !AB1;
            F_I = 0;
            //printf("1 Hubo un cambio, rey.\n");
        }
        if(F_I2 == 1){
            // Accion a ejecutar con el interruptor 2
            AB2 = !AB2;
            F_I2 = 0;
            //printf("2 Hubo un cambio, rey.\n");
        }
        if(F_I3 == 1){
            // Accion a ejecutar con el interruptor 2
            AB3 = !AB3;
            F_I3 = 0;
            //printf("3 Hubo un cambio, rey.\n");
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
    if(digitalRead(INTE3) != E_I3){
        F_I3 = 1;
        E_I3 = !E_I3;
    }
    
    usleep(10000);
}

¡¡Buenas tardes!!

Este archivo TXT contiene todas las instrucciones sobre como ejecutar todos los programas de las diferentes partes
que el sistema SCADA implementado (a pequeña escala) contiene.

/*
------------------------------------------------------------------------------------------------------------
UTR (Raspberry Pi's)
------------------------------------------------------------------------------------------------------------
*/
Para poder ejecutar de manera exitosa el programa llamado "UTR.c", es necesario compilarlo desde una terminal
de la forma siguiente:

gcc UTR.c -o *Cualquier nombre que se le quiera poner al ejecutable* -lwiringPi -lwiringPiDev -lpthread

Al terminar de compilarlo, se creará, en el mismo directorio donde se tiene el archivo "UTR.c", un ejecutable
con el nombre que se le quiso poner al momento de compilarlo. De esta manera se tiene listo el archivo para 
ser ejecutado por la Raspberry Pi. 

Este procedimiento se debe seguir para ambas Raspberry Pi's del sistema SCADA implementando.


/*
------------------------------------------------------------------------------------------------------------
Historiador (MATLAB)
------------------------------------------------------------------------------------------------------------
*/
Para poder ejecutar de manera apropiada el programa "App_historiador.mlapp", es necesario abrir MATLAB. Desde
MATLAB nos vamos a la pestaña de "APPS" y ahí buscamos el icono que dice "Desing App". Desde App Designer, 
seleccionamos la opción que dice "Open...". Desde esa pestaña seleccionamos la carpeta donde se encuentre el archivo
"App_historiador.mlapp" y cuando este abra, presionar el botón verde que dice "RUN". De esta manera se ejecuta
la interfaz gráfica correspondiente al historiador, el cual al momento de presionar el switch en la esquina 
posterior izquierda y ver que se coloca en la posición de "On", comenzará a recibir datos e interactuar con los UTRs

/*
------------------------------------------------------------------------------------------------------------
ADC (Arduino Nano)
------------------------------------------------------------------------------------------------------------
*/
Desde la IDE de Arduino, conectamos nuestro Arduino Nano hacia cualquier puerto deseado. Desde ahí seleccionamos
la opción de subir, la cual compilará y subirá hacia el Arduino Nano, el programa para poder realizar la lectura
de ADC y enviarla a la Raspberry Pi por medio de SPI.


/*
------------------------------------------------------------------------------------------------------------
Evento IOT (ESP32)
------------------------------------------------------------------------------------------------------------
*/
Para realizar el evento IOT, es necesario instalar todos los paquetes necesarios para que la IDE de Arduino detecte 
al ESP32. Una vez se realiza esto, dentro del programa, se deben cambiar ciertos parámetros para garantizar su 
funcionalidad.

1. Primero, se debe cambiar la línea 18 que contiene la variable tipo const char* ssid, la cual contendrá el 
nombre de la Red WIFI a la que nos queremos conectar.
2. Segundo, se debe cambiar la línea 19 que contiene la variable tipo const char* password, la cual contendrá 
la contraseña de la Red WIFI a la que nos conectaremos.

Una vez se cambian estos parámetros, presionamos el símbolo que contiene una pequeña flecha. Esta opción compilará
y subirá el programa a nuestro ESP32. Esperamos a que suba y una vez se encuentra subido, abrimos la terminal, 
presionamos el botón de "EN" integrado en nuestra placa ESP32 y en la terminal mostrará la IP local a la que nos
debemos de dirigir desde nuestro navegador de conveniencia. 

NOTA: Tanto la computadora, teléfono o dispositivo con conexión a internet, que utilicemos para visualizar
nuestro servidor web, deben de estar conectados a la misma RED a la que está conectado el ESP32

------------------------------------------------------------------------------------------------------------
Una vez se ejecutan todos los programas y se encuentran corriendo, el sistema funciona correctamente. Se recomienda 
que el servidor WEB (ESP32), el historiador y las UTRs se encuentren conectados a la misma red para que se puedan 
utilizar los web sockets sin problema alguno, al mismo tiempo que se visualiza el evento IOT a través del
servidor web que genera el ESP32. Si no se puede conectar a la misma RED por problema de restricciones, se recomienda
que el servidor WEB se encuentre en una red distinta a la red a donde se encuentra conectado el historiador junto
a los UTRs. Si no existen restricciones dentro de la RED, por facilidad, se recomiendan colocar todos los dispositivos
dentro de la misma RED. 

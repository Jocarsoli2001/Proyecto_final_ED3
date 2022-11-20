#include <SPI.h>
int Pot = A0;
int PotVal = 0;
int msm = 0;
void setup() {
 // Serial.begin(9600);
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPI.attachInterrupt();
}
// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;
  SPDR =  msm;
}  // end of interrupt service routine (ISR) for SPI
void loop () {
     PotVal = analogRead(Pot);
     msm = PotVal/4;
     
    // Serial.println(PotVal);
}

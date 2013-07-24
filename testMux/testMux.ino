
#include <stdarg.h>
#include <stdio.h>


const int muxPin = A0;
const int tempMux = 1 << 5 ;
const int resistanceMux = 1 << 5;
const int pressureMux = 1<< 5;

int muxChoice = 0;


void p(char *fmt, ... ){
  
  int buffer = 1024;
  
  char tmp[buffer]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, buffer, fmt, args);
  va_end (args);
  Serial.print(tmp);
}

void jsonprint(char * key, float value, unsigned long time){
  p("{\"%s\": %f, \"time\": %u}\n", key, value, time);
}

int my_putc( char c, FILE *t) {
  Serial.write( c );
}
void setup(){
  Serial.begin(115200);
  DDRD = B11111000; // set digital ports 3-7 as writes for mux
  fdevopen( &my_putc, 0);

}

void loop(){
  samplePorts();
  delay(1000);
}


void samplePorts() /*sample ports and send over serial according to rates*/ 

{
    switch(muxChoice){
    case 0: 
      PORTD = tempMux; 
      break;
    case 1: 
      PORTD = resistanceMux; 
      break; 
    case 2: 
      PORTD = pressureMux; 
      break;
    }
    
    int muxVal = analogRead(muxPin);

    switch(muxChoice){
    case 0: 
      printf("{\"temperature\": %d, \"time\": ", muxVal);
      Serial.print(millis());
      Serial.println("}");
      break;
    case 1: 
      printf("{\"resistance\": %d, \"time\": ", muxVal);
      Serial.print(millis());
      Serial.println("}");  
      break; 
    case 2: 
      printf("{\"pressure\": %d, \"time\": ", muxVal);
      Serial.print(millis());
      Serial.println("}");
      break;
    }

    muxChoice = (muxChoice+1)%3; 
}








#include <stdarg.h>
#include <stdio.h>


const int muxPin = A0;
const int capacitorAnalogPin1 = A1;
const int capacitorAnalogPin2 = A2;

const int imuPinX = A3;
const int imuPinY = A4;
const int imuPinZ = A5;

const int tempMux = 0 ;
const int resistanceMux = 1 << 5;
const int pressureMux = 2 << 5;

const int capacitorChargePin1 = 3;
const int capacitorChargePin2 = 4;

const int imuRate = 5.; /* sampling rate in Hz */
const float capacitorRate = .1; //dedicate to capacitor every  10 seconds
const int muxRate = 3.0;

int muxChoice = 0;
const int capacitorResistorValue = 1000;

unsigned long imuTime = 0;
unsigned long capacitorTime = 0;
unsigned long capacitorStartTime = 0;
unsigned long muxTime = 0;

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
  DDRD = B11111000; // set digital ports 3-7 as writes for mux and capacitor
  digitalWrite(capacitorChargePin1, LOW); //discharge capacitor
  digitalWrite(capacitorChargePin2, LOW); //discharge capacitor
  fdevopen( &my_putc, 0);

}

void loop(){
  samplePorts();
}

void capacitorRead(){
  const int length = 200;
  int capacitor1Data [length];
  int capacitor2Data [length];
  long capacitorTimeData[length];
  int del = 0;
  int del2 = 0;

  long looptimestart = micros();
  while(analogRead(capacitorAnalogPin1) > 0){         // wait until capacitor is completely discharged

    if (micros() - looptimestart > 10000000*2){ //too long in loop (infinite loop threat)
      //Serial.print("break discharge, analog: " + analogRead(capacitorAnalogPin));
      break;
    }
  }
  long startTime = micros();
  looptimestart = micros();
  int index = 0;
  int cap1Value = 0;
  int cap2Value = 0;
  digitalWrite(capacitorChargePin1, HIGH);
  digitalWrite(capacitorChargePin2, HIGH);

  do{       // 647 is 63.2% of 1023, which corresponds to full-scale voltage 
    
   cap1Value = analogRead(capacitorAnalogPin1);
   cap2Value = analogRead(capacitorAnalogPin2);
    long tt = micros() - looptimestart;  
    capacitor1Data[index] = cap1Value;
    capacitor2Data[index] = cap2Value;
    capacitorTimeData[index] = tt;
    index++;

    delay(del2);
    //Serial.print(index);
    if (tt - looptimestart > 1000000*2){ //too long in loop (infinite loop threat)
      //Serial.print("break charge loop, analog: " + analogRead(capacitorAnalogPin));
      break;
    }
  }
  while ( (cap1Value < 800 || cap2Value < 800) && index <= length);

  delay(del);
  Serial.print("{\"capacitor1\": [");
  Serial.print(capacitor1Data[0]);

  delay(del);
  for (int i = 1; i< index;i++){
    delay(del);
    Serial.print(", ");
    Serial.print(capacitor1Data[i]);    

  }
  delay(del);

  Serial.print("], \"capacitor2\": [");
  Serial.print(capacitor2Data[0]);
  for (int i = 1; i< index;i++){
    delay(del);
    Serial.print(", ");
    Serial.print(capacitor2Data[i]);       
  }
  delay(del);
  Serial.print("],");
  Serial.print("\"capacitorIime\": [");
  Serial.print( capacitorTimeData[0]);
  for (int i = 1; i< index;i++){
    delay(del);
    Serial.print(", ");
    Serial.print(capacitorTimeData[i]);
  }
  delay(del);
  Serial.print("]}\n");
  /* dicharge the capacitor  */
  digitalWrite(capacitorChargePin1, LOW);             // set charge pin to  LOW 
  digitalWrite(capacitorChargePin2, LOW);             // set charge pin to  LOW 
}  

void samplePorts() /*sample ports and send over serial according to rates*/ 

{
  unsigned long time = millis();
  capacitorRead();
}








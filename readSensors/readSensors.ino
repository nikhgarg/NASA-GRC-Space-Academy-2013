
#include <stdarg.h>
#include <stdio.h>


const int muxPin = A0;
const int capacitorAnalogPin1 = A1;
const int capacitorAnalogPin2 = A2;

const int imuPinX = A3;
const int imuPinY = A4;
const int imuPinZ = A5;

const int tempMux = 0;
const int resistanceMux = 1;
const int pressureMux = 2;

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
  DDRD = DDRD | B11100000; // set digital ports 5-7 as writes for mux
  pinMode(capacitorChargePin1, OUTPUT); 
  pinMode(capacitorChargePin2, OUTPUT); 
  digitalWrite(capacitorChargePin1, LOW); //discharge capacitor
  digitalWrite(capacitorChargePin2, LOW); //discharge capacitor
  fdevopen( &my_putc, 0);

}

void loop(){
  samplePorts();
}

void capacitorRead(){
  const int length = 200;
  printf("in capacitor read");
  int capacitor1Data [length] = {0};
  int capacitor2Data [length] = {0};
  long capacitorTimeData[length] = {0};

  printf("after allocation");

  long looptimestart = micros();
  while(analogRead(capacitorAnalogPin1) > 0){         // wait until capacitor is completely discharged

    if (micros() - looptimestart > 1000*2){ //too long in loop (infinite loop threat)
      //Serial.print("break discharge, analog: " + analogRead(capacitorAnalogPin));
      break;
    }
  }
  printf("discharged capacitor");
  PORTD = capacitorChargePin1 | capacitorChargePin2; // set capacitorChargePin HIGH and capacitor charging
  long startTime = micros();
  looptimestart = micros();

  int index = 0;
  int cap1Value = 0;
  int cap2Value = 0;

  do{       // 647 is 63.2% of 1023, which corresponds to full-scale voltage 

    cap1Value = analogRead(capacitorAnalogPin1);
    cap2Value = analogRead(capacitorAnalogPin2);
    long tt = micros();  
    capacitor1Data[index] = cap1Value;
    capacitor2Data[index] = cap2Value;
    capacitorTimeData[index] = tt;
    index++;

    delay(10);

    if (tt - looptimestart > 1000*2){ //too long in loop (infinite loop threat)
      //Serial.print("break charge loop, analog: " + analogRead(capacitorAnalogPin));
      break;
    }
  }
  while ( cap1Value < 800 || cap2Value < 800);
  delay(100);
  printf("{\"capacitor1\": [%d", capacitor1Data[0]);
  for (int i = 1; i< index;i++){
    delay(100);
    printf(", %d", capacitor1Data[i]);    

  }
  delay(10);

  printf("],\"capacitor2\": [%d", capacitor2Data[0]);
  for (int i = 1; i< index;i++){
    delay(10);

    printf(", %d", capacitor2Data[i]);    
  }
  delay(10);

  printf("],\"capacitorIime\": [%d", capacitorTimeData[0]);
  for (int i = 1; i< index;i++){
    delay(10);

    printf(", %l", capacitorTimeData[i]);    
  }
  delay(10);

  printf("],\"time\": %l\n", millis());

  /* dicharge the capacitor  */
  digitalWrite(capacitorChargePin1, LOW);             // set charge pin to  LOW 
  digitalWrite(capacitorChargePin2, LOW);             // set charge pin to  LOW 
}  

void samplePorts() /*sample ports and send over serial according to rates*/ 

{
  unsigned long time = millis();

  if (time - imuTime > 1000/imuRate) /* sample imu pins */ {
    unsigned long t = millis();
    int imuX = analogRead(imuPinX);
    int imuY = analogRead(imuPinY);
    int imuZ = analogRead(imuPinZ); 

    p("{\"imux\": %d, \"imuy\": %d, \"imuz\": %d, \"time\": %u}\n", imuX, imuY, imuZ, time);

    imuTime = time;
  }

  if (time - capacitorTime > 1000/capacitorRate) /* sample capacitorPin */ {
    capacitorRead();
    capacitorTime = millis();
  }

  if (time - muxTime > 1000/muxRate) /* sample mux */ {

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
    muxTime = time;

    switch(muxChoice){
    case 0: 
      jsonprint("temperature", muxVal, millis());
      break;
    case 1: 
      jsonprint("resistance", muxVal, millis());
      break; 
    case 2: 
      jsonprint("pressure", muxVal, millis());
      break;
    }

    muxChoice = (muxChoice+1)%3; 

  }

}







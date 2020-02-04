#include <Servo.h>

//interrupteurs de choix de mode
const int mode1 = 13;
const int mode2 = 12;
const int mode3 = 11; 
//const int mode4 = 10;

//photorésistances
const int ambient = A3;
const int leftpr  = A1;
const int rightpr = A2;

//Direction & Propultion
Servo servo;
const int PWM_for  = 6;
const int PWM_bac  = 5;

//Avertisseurs
const int ledPin  = 8;
const int led2    = 10;
const int speaker = 7;

//Ultra son
const int TrigFront = 4;
const int TrigBack  = A4;
const int EchoFront = 3;
const int EchoBack  = A5;





//------------------------------------Prototypes---------------------------------------
void lka_func(Servo servo);
void back_func(int trig, int echo, int pwmforwards, int pwmbackwards, int pinled);
void ebs_func(int trig, int echo, int forw, int back);
void transition(int azerty, int pwmpin, int duration, int a, int b);
void blink_headlights();
//-------------------------------------------------------------------------------------





void setup(){
  Serial.begin(9600);
  
  pinMode(mode1, INPUT_PULLUP);
  pinMode(mode2, INPUT_PULLUP);
  pinMode(mode3, INPUT_PULLUP);
//  pinMode(mode4, INPUT_PULLUP);
  
  pinMode(ambient, INPUT_PULLUP);
  pinMode(leftpr, INPUT_PULLUP);
  pinMode(rightpr, INPUT_PULLUP);
  
  servo.attach(9);
  pinMode(PWM_for, OUTPUT);
  pinMode(PWM_bac, OUTPUT);
  digitalWrite(PWM_for, LOW);
  digitalWrite(PWM_bac, LOW);
  
  pinMode(EchoBack, INPUT);
  pinMode(EchoFront, INPUT);
  pinMode(TrigFront, OUTPUT);
  pinMode(TrigBack, OUTPUT);
  digitalWrite(TrigFront, LOW);
  digitalWrite(TrigBack, LOW);

  pinMode(ledPin, OUTPUT);
  pinMode(speaker, OUTPUT);
  pinMode(led2, OUTPUT);

  servo.write(30);
  delay(500);
  //aaaaaaa
}

void loop(){

    Serial.println(analogRead(ambient));
  if(analogRead(ambient)>=120){
    digitalWrite(led2, HIGH);
  }
  else{
    digitalWrite(led2, LOW);
  }
  //----------------------------MODE 1: Lane Keep Assist (LKA)-----------------------------
  if(digitalRead(mode1) == LOW){
    Serial.println("mode 1!");
    lka_func(servo);
  }
  //---------------------------------------------------------------------------------------

  //-------------------------------MODE 2: Freinage d'urgence------------------------------
  else if(digitalRead(mode2)==LOW ){
     ebs_func(TrigFront, EchoFront, PWM_for, PWM_bac);
    }
    
  //---------------------------------------------------------------------------------------

  //---------------------------------MODE 3: Radar de recul--------------------------------
  else if(digitalRead(mode3)==LOW){  
   Serial.println("mode 3");
   back_func(TrigBack, EchoBack, PWM_for, PWM_bac, ledPin);
  }
  //---------------------------------------------------------------------------------------
  else {
    digitalWrite(PWM_for,LOW);
    digitalWrite(PWM_bac,LOW);
  }
}

//------------------------------------------------------------------------------------------
void back_func(int trig, int echo, int pwmforwards, int pwmbackwards, int pinled){

const unsigned long MEASURE_TIMEOUT = 25000UL;
const float sound_speed = 340.0/1000;
const byte TRIGGER_PIN = trig;
const byte ECHO_PIN = echo;
const int pwma=pwmforwards;
const int pwmb=pwmbackwards;
int ledpin = pinled;
int fin = 0;
int fin2 = 0;

digitalWrite(TRIGGER_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIGGER_PIN, LOW);

long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
float distancemm = measure / 2.0 * sound_speed;
float distancecm = distancemm/10.0;


Serial.print("mode 3 Distance (cm): "); Serial.println(distancecm);

if(distancecm<=8 ){     
    
    transition(1, pwmb, 5, 170, 110);
    delay(200);
    analogWrite(pwmb, 0);
    digitalWrite(ledpin, HIGH);
    delay(distancemm/3);
    digitalWrite(ledpin,LOW);
    delay(distancemm/4);
  }  
  else{
    fin = 0;
    analogWrite(pwmb, 170);
    digitalWrite(ledpin, HIGH);
    delay(distancemm/2);
    digitalWrite(ledpin,LOW);
    delay(distancemm/2);
  }
  delay(500);
}

//------------------------------------------------------------------------------
void ebs_func(int trig, int echo, int forw, int back){

  const unsigned long MEASURE_TIMEOUT = 25000UL;
  const float sound_speed = 340.0/1000;
  const byte TRIGGER_PIN = trig;
  const byte ECHO_PIN = echo;
  const int pwma=forw;
  const int pwmb=back;
  int fin = 0;
  int fin2 = 0;

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
  float distancemm = measure / 2.0 * sound_speed;
  float distancecm = distancemm/10.0;


  Serial.print("Distance (cm): "); Serial.println(distancecm);

  
  if(distancecm<10){
    transition(1, pwma, 70, 255, 100);
    digitalWrite(pwma,LOW);
    blink_headlights();
  }
  else{ 
    analogWrite(pwma, 255);
  }
  delay(500);
}

//------------------------------------------------------------------------------
void lka_func(Servo servo){

  int val1 = analogRead(A1);
  int val2 = analogRead(A2);
  int limit = 200;
  
 servo.write(30);
 Serial.print(val1);Serial.print("  -  ");Serial.print(val2);
  if(val1>=limit){
    servo.write(50);
    delay(100);
  }
  if(val2>=limit){
    servo.write(10);
    delay(100);
  }
  if(val2>=limit && val1>=limit){
   servo.write(30);
   delay(100);
  }
  delay(300);
}

//------------------------------------------------------------------------------
void transition(int azerty, int pwmpin, int duration, int a, int b){

  int finfunc = 0;
  
  if(finfunc == 0){
    if(azerty==1){
    //ralentir (avant)    
      for(int i=a; i<=b; i++){
        analogWrite(pwmpin, i);
        delay(duration);
      }
    }
    else{
    //ralentir (arrière)
      for(int i=a;i>=b;i--){
        analogWrite(pwmpin, i);
        delay(duration);
      }
    }
    finfunc = 1;
  }
}

//-----------------------------------------------------------------------
void blink_headlights(){
  int finfunc = 0;
  delay(500);
  if(finfunc == 0){
    for(int j = 0; j<=4; j++){
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);    
    }
    finfunc++;
  }
}

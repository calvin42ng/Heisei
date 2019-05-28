// Using technique from here https://learn.adafruit.com/multi-tasking-the-arduino-part-3/using-neopatterns 
//    see link above for a good tutorial on this multi-task-ish way of doing things.

// Flicker Realistic Candle
// Keith Kelly 2019-03-04 www.KeithsTestGarage.com
// Milestone 2 of 3


#ifdef __AVR__
#include <avr/power.h>
#endif


// NeoPatterns will require the Adafruit NeoPixel library.  Be sure to install that.  
#include "NeoPatterns.h"


#define NEO_PIN 5    // First RGBW Strip Pin
#define NEO2_PIN 9   // Second RGBW Strip Pin
#define NEO_COUNT 10  // First RGBW Strip Count
#define NEO2_COUNT 10 // Second RGBW Strip Count
#define MIC_PIN A1   // Microphone Pin
#define FLAME_PIN A3 // IR Sensor Pin

//#define FLAME_DURATION 2000 // millis- duration of flame before lighting candle
//#define FLAME_THRESHOLD 500 // 0...1023 - analog reading of FLAME value to detect
#define MIC_THRESHOLD 250 // 0...1023 - analog reading of MIC value to detect

void Strip1Complete(); 
void Strip2Complete();

NeoPatterns Strip1(NEO_COUNT, NEO_PIN, NEO_RGBW + NEO_KHZ800, &Strip1Complete);
NeoPatterns Strip2(NEO_COUNT, NEO2_PIN, NEO_RGBW + NEO_KHZ800, &Strip2Complete);

uint32_t baseColor; // What is the ON resting color?
uint32_t offColor;  // What is the OFF resting color?


// Vars to keep track of things
bool flaming = false;

bool lit = false;
//int flame_val = 0;
int mic_val = 0;
byte rnd = 0;

// Vars for tracking pseudo-async times
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long flameStartedMillis = 0;        // will store last time LED was updated

//int arrayTemp[9];
int arrayTempBig[200];
int arrayTempBig2[200];
int mm = 0;

int arrayTempA[9];
int arrayTempB[9];
int j_A = 0;
int j_B = 0;


int k = 0; // same as the funtion of 'int j'
//int q = 0;

char arrayUse[9];

String inString = "";
int j = 0;
int recSign = 10;

void setup(){

    Serial.begin(9600); // uncomment this to figure sensor readings.
    
    // initialize pins
    pinMode(FLAME_PIN, INPUT);
    pinMode(MIC_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT); 

    // Make sure the following pins are OFF
    digitalWrite(LED_BUILTIN, LOW); 


    // Initialize NeoPixel Strip 1
    Strip1.begin();
    Strip1.show();
    baseColor = Strip1.Color(2, 110, 0, 100);// GRBW
    Strip1.Color1 = baseColor;

    // Initialize NeoPixel Strip 2
    Strip2.begin();
    Strip2.show();
    baseColor = Strip2.Color(2, 110, 0, 100);// GRBW
    Strip2.Color1 = baseColor;
    
    blowOutCandle();  // Make sure candle is off 
    
}



int breakTag1 = 0;

int mark1 = 0;
int mark2 = 0;


int sentVer = 0;
void loop(){ 
  
//    for(pvt = 0; pvt < 50; pvt++){
//      Serial.write(arrayTempA[pvt]);
//    }
    


  
//  Serial.write(arrayTempB[sentVer++]);
//  if(sentVer >= 9) sentVer = 0;
   

   
   
    
   checkSensors(); // Check for fire and strong winds

   if(Serial.available() > 0){
    recSign = 0;
    mark1 = 0;
    mark2 = 0;
    //arrayTemp[4] = 4;
    
//        for(int i = 0; i < 9; i++){
//          arrayTemp[i] = 0;
//        }

        for(int i = 0; i < 9; i++){
          arrayTempA[i] = 0;
        }

        for(int i = 0; i < 9; i++){
          arrayTempB[i] = 0;
        }                   
        
        for(int i = 0; i < 200; i++){
          arrayTempBig[i] = 0;
        } 
        
        for(int i = 0; i < 200; i++){
          arrayTempBig2[i] = 0;
        }         
    breakTag1 = 0;       
        
        
     while (Serial.available() > 0)  
      { 

        if(j > 9) j = 0;
        if(k >= 200) k = 0;
        int inChar = Serial.read();
        //Serial.write(inChar);
        //arrayTemp[j++] = inChar - 48;
        arrayTempBig[k++] = inChar - 48;
        
        

        recSign = 1;
      }
    process();  
    }
    if(recSign == 1){
    
    blowOutCandle(); // Turn off light 
    lit = false;

    Strip1Complete(); 
    Strip2Complete();
    
    recSign = 10;    
    j = 0;  
    }
     
     
     

    Strip1.Update();
    Strip2.Update();

    
}

// Checks for fire and strong winds every 50 milliseconds
void checkSensors(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 50) { // only check every 50ms
   // flame_val = analogRead( FLAME_PIN );
    mic_val = analogRead( MIC_PIN );
    previousMillis = currentMillis;
    
    //  Uncomment below to check sensor readings.  
     //   Serial.println(mic_val);
   //     Serial.println(flame_val);
  }

  

  if (mic_val > MIC_THRESHOLD){ // MIC REGISTERED SOUND.  BLOW OUT.
    blowOutCandle(); // Turn off light 
    lit = false;
  }

  if (!lit){ // if candle isn't lit
    //if (flame_val > FLAME_THRESHOLD)  { // if flame is detected
    if (mic_val < MIC_THRESHOLD)  {    
        // record the starting time (if flame not yet detected)
        if (!flaming){  // if not flaming
          flaming = true; 
          flameStartedMillis = millis();  // record time that flaming begins
          lightCandle(); // ok finally light it up. 
          lit = true;          
        }
//        else if (checkFlame()){ // has flame been flaming long enough?          
//          lightCandle(); // ok finally light it up. 
//          lit = true; 
//        }
    }
//    else{ // no flame detected
//      flaming = false; 
//    }
  }  
  
}


// Check if flame has been detected long enough.
//bool checkFlame(){    
//  unsigned long currentMillis = millis(); // get current time
//  if (currentMillis - flameStartedMillis >= FLAME_DURATION) {
//    return true;  
//  }
//  else
//    return false;
//}


//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------

// The below configuration routines are the same except for the strip
// being used.  I kept them separate so the strips can be configured
// with different ranges if desired.

void Strip1Complete(){
    Strip1.Reverse();

    if (Strip1.Direction == REVERSE){ // go back down
        Strip1.Interval = random(5,22); // choose random speed in range
    }
    else{  // pattern ended.  Stop, then trigger re-flicker
        
        /*  This needs an explanation:  
            
              rnd = random(random(45,55),random(80,255));

            You'd think that we could just do random(45,255), right?

            Selecting the low and high randoms first helps make sure that
            the ending random number is *more likely* to fall within a limited range.  

            When observing the flickering of a real candle, the flame generally bounces
            around in a certain range.  But, every once in a while, it spikes high or low.

            This random(random, random) solves that problem!  #proudnerddadmoment
        */
        rnd = random(random(45,55),random(80,255)); 

        //Strip1.Pixel = random(0,Strip1.numPixels()); // pick a random Pixel
        Strip1.Pixel = arrayTempB[random(0,9)]; // pick a random Pixel
        Strip1.Interval = 1; 
        Strip1.Color2 = Strip1.Color(2,rnd,0,rnd-10); //GRBW random red and random white-10
    }
}

void Strip2Complete(){
    Strip2.Reverse();

    if (Strip2.Direction == REVERSE){ // go back down        
        Strip2.Interval = random(5,22); // choose random speed in range
    }
    else{  // pattern ended.  Stop, then trigger re-flicker
        rnd = random(random(45,55),random(80,255)); // see explanation in Strip1Complete().
        //Strip2.Pixel = random(0,Strip2.numPixels()); // pick a random Pixel
        Strip2.Pixel = arrayTempA[random(0,9)];
        Strip2.Interval = 1;
        Strip2.Color2 = Strip2.Color(2,rnd,0,rnd-10); //GRBW random red and random white-10
    }
}

// (pseudo)ASYNC light the candle by starting the animations
void lightCandle(){
    Strip1.Flicker(Strip1.Color1, Strip1.Color2, 20, 5);
    Strip2.Flicker(Strip2.Color1, Strip2.Color2, 20, 5);
}

void blowOutCandle(){    
  cWipe(offColor, 10); // synchronously turn the candle off.

  // We don't want no stinking patterns.
  Strip1.ActivePattern = NONE; 
  Strip2.ActivePattern = NONE;
  flaming = false;
}



// synchronous color wipe
void cWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < Strip1.numPixels(); i++) {
    Strip1.setPixelColor(i, c);
    Strip1.show();
    delay(wait);
  }
  for (uint16_t i = 0; i < Strip2.numPixels(); i++) {
    Strip2.setPixelColor(i, c);
    Strip2.show();
    delay(wait);
  }
}


void process(){
  if(breakTag1 == 0){
      for(int m = 0; m < 200; m++){
        if(arrayTempBig[m] != 0){ // minus 0
          arrayTempBig2[mm] = arrayTempBig[m];
          mm += 1;
          if(mm >= 200){
            mm = 0;
            break;
          }
         } 
       }
       breakTag1 = 1;
  }
     
  for(int i = 0; i < 200; i++){
   // Serial.write(arrayTempBig2[i]);
    if(arrayTempBig2[i] == 17) {
        mark1 = i;  
      }
    if(arrayTempBig2[i] == 18) {
        mark2 = i;
      }
   // Serial.write(arrayTempBig2[i]);
   // Serial.write(mark1);
   // Serial.write(mark2);
   }
  for(int i = mark1+1;i<mark2 ;i++){
    if(j_B >= 9) j_B = 0; // =?
    arrayTempB[j_B++] = arrayTempBig2[i];
  }

  for(int i = mark1-1;arrayTempBig2[i]!=0; i-- ){
    
    if(j_A >= 9) j_A = 0;
    arrayTempA[j_A++] = arrayTempBig2[i];
    
  }

}

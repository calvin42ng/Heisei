import processing.net.*; 

import oscP5.*;
import netP5.*;

// Light  
import processing.serial.*;
Serial myPort;


//the second processing programme
import hypermedia.net.*;
UDP udp;
String ip = "localhost";
int port = 6100;
//-----

OscP5 oscP5;
NetAddress myRemoteLocation;

Client myClient; 
String dataIn; 

PImage img;
String url = "";

int number;
int preNumber = 100;
String numberString;

int resultNum;

float timer = 0;

int[] array1 = new int[16];

// Light
String temp;
String message;

String sendAD;
String sendAD2;
int indexAD = 0;
int indexAD2 = 0;

int val;
//-------

void setup() { 
  size(8,2);
  background(255);
  myClient = new Client(this, "127.0.0.1", 9090); 
  
  oscP5 = new OscP5(this,8000);
  myRemoteLocation = new NetAddress("127.0.0.1",5555);
  
  
  udp = new UDP( this, 6000 );
  udp.listen( true );

  
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);  
   
  sendAD = "";
  sendAD2 = "";
} 
 
void draw() { 

  if (myClient.available() > 0) { 
    dataIn = myClient.readString();
    background(0);

    if(dataIn != null){
      if(dataIn.charAt(0) == 'I' && dataIn.charAt(1) == 'D' && dataIn.charAt(2) == 'P'){
        String result = dataIn.substring(3,dataIn.length());
        resultNum = Integer.parseInt(result);
        print("Now the result number is:", resultNum, "\n");  
        
      }
      else{
        print(dataIn, '\n');
        url = dataIn;
        img = loadImage(url, "jpg");
        
        udp.send( url, ip, port );
        
        img.resize(8, 2);
        loadPic();
        }
      }
  }
  timer = (millis()/1000.0) % 10.0;
  if(timer > 9.98){
    timer = 0;
    number = (int)random(0,resultNum-1);
    for(; number == preNumber ;){
      number = (int)random(0,resultNum-1);
    }
    print(number, '\n');
    numberString=String.valueOf(number);
    myClient.write("o");
    delay(1000);
    myClient.write(numberString);
    preNumber = number;
    
    // send to pure data
    OscMessage myMessage = new OscMessage("/test");
    myMessage.add(array1);
    oscP5.send(myMessage, myRemoteLocation);
    
  }
  
  
    while (myPort.available() > 0) {
    int inByte = myPort.read();
    println("what I received: ", inByte);
  } 
  
} 


void keyPressed() {
  //quit
  if(key == 'q' || key == 'Q'){
    myClient.write("q");
  }
  
  if(key == 's' || key == 'S'){
    myClient.write("s");
  }
  
  
}

void loadPic(){
  loadPixels();
  img.loadPixels();
  for (int x = 0; x < width; x++){
    for (int y = 0; y < height; y++){
      int loc = x + y * width;
      float b = brightness(img.pixels[loc]);
      if (b > 160) {  //160
        pixels[loc] = color(255);
        array1[loc] = 1;
        if(loc <= 7){
          println("first location:", loc);
          sendAD = sendAD + (loc + 1 + "");
        }
        else{
          int loc2;
          loc2 = loc - 8;
          println("Second location:", loc2);
          sendAD2 = sendAD2 + (loc2 + 1 + "");
        }
      }
      else{
        pixels[loc] = color(0);
        array1[loc] = 0;
      }
    }
  }
  updatePixels();
  printArray(array1);
  sendAD = sendAD + sendAD + sendAD;
  sendAD2 = sendAD2 + sendAD2 + sendAD2;
  println("sendAD content: ", sendAD);
  println("sendAD2 content: ", sendAD2);

  
  myPort.write(sendAD + 'A' + sendAD2 + 'B'); 
  sendAD = "";
  sendAD2 = "";

}

void oscEvent(OscMessage theOscMessage) {
  print("### received an osc message.");
  print(" addrpattern: "+theOscMessage.addrPattern());
  println(" typetag: "+theOscMessage.typetag());
}

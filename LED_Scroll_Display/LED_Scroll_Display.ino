
//Libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <dht.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

//Constants
SoftwareSerial btSerial(3, 4); // RX, TX
const int photoCell = A0;
dht DHT;
const int DHT22_PIN = 9; // Data pin of DHT 22 (AM2302)
RTC_DS1307 rtc;
const int pinCS = 10; 
const int numberOfHorizontalDisplays = 10;
const int numberOfVerticalDisplays = 1;
const int wait = 20; // In milliseconds
const int spacer = 1;
const int width = 5 + spacer; // The font width is 5 pixels
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

//Variables
int chk, length, brLevel, photoCellValue, count=0;
String msg;
boolean autoBR=true;
boolean messageCompleted=false;
boolean newMessage=false;
String tickerText = "Insert Ticker Message";
char incomingByte; 
String command;
unsigned long previousMillis = 0;       

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  rtc.begin();  
  matrix.setIntensity(1); // Use a value between 0 and 15 for brightness
}

void loop() {
  initMatrix();
  unsigned long currentMillis = millis();
  communication();
  
  //Show content 
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    count++; //Seconds
  }
  //Display the ticker message
  if (count>=0){
    scroll();
    count=0; //<----reset
  }
}
//Bluetooth communication
void communication(){
  if (btSerial.available()){
    incomingByte = btSerial.read();
    if(incomingByte=='>'){
       messageCompleted=true;
       newMessage=false;
    }
    else if (incomingByte=='<'){
       newMessage=true;
    }

    if (newMessage){
       command.concat(incomingByte);
    }
  }

  if(messageCompleted){
    //Brightness level
    if (command.charAt(1)=='B'){
      if (command.substring(2)=="Auto"){
        autoBR=true;
      }
      else{
        autoBR=false;
        brLevel= (command.substring(2)).toInt() - 1;
        
      }
    }
    
    //Update ticker text
    else if (command.charAt(1)=='t'){
      tickerText=command.substring(2);   
      Serial.println(tickerText);   
    }
    command="";
    messageCompleted=false;
  }
}
//Control brightness 
void controlBR(){
  if (autoBR){
    photoCellValue = analogRead(photoCell);
    photoCellValue = map(photoCellValue,1023,0,0,15); 
    matrix.setIntensity(photoCellValue); // Use a value between 0 and 15 for brightness
  }
  else{
    matrix.setIntensity(brLevel); // Use a value between 0 and 15 for brightness
  }
}

//Ticker Text
void scroll(){
  for ( int i = 0 ; i < width * tickerText.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tickerText.length() ) {
        matrix.drawChar(x, y, tickerText[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

    delay(wait);
  }
}
//Control function for display 
void fullOn(){
  matrix.fillScreen(HIGH);
  matrix.write();
}
void fullOff(){
  matrix.fillScreen(LOW);
  matrix.write();
}
void initMatrix(){
  matrix.setPosition(0, 9, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 8, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 7, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 6, 0); // And the last display is at <3, 0>
  matrix.setPosition(4, 5, 0); // And the last display is at <3, 0>
  matrix.setPosition(5, 4, 0); // And the last display is at <3, 0>
  matrix.setPosition(6, 3, 0); // And the last display is at <3, 0>
  matrix.setPosition(7, 2, 0); // And the last display is at <3, 0>
  matrix.setPosition(8, 1, 0); // And the last display is at <3, 0>
  matrix.setPosition(9, 0, 0); // And the last display is at <3, 0>
  /*For rotation change the second number as:
   * 0: No rotation
   * 1: 90 deg clockwise
   * 2: 180 deg 
   * 3: 90 deg counter clockwise
    */
  matrix.setRotation(0, 0);
  matrix.setRotation(1, 0);
  matrix.setRotation(2, 0);
  matrix.setRotation(3, 0);
  matrix.setRotation(4, 0);
  matrix.setRotation(5, 0);
  matrix.setRotation(6, 0);
  matrix.setRotation(7, 0);
  matrix.setRotation(8, 0);
  matrix.setRotation(9, 0);

}


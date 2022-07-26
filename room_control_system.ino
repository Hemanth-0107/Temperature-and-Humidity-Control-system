
const int temp_trans_pin=A0,heater_pin=13,fan_pin=6,switchPin=7;
float mintemp=16,maxtemp=28;
#include<LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>
LiquidCrystal LCD{12,11,5,4,3,2};
#define dryThreshold 50 //below this value, begin alerting dry, turn red;
#define wetThreshold 200 //above this value, begin alerting wet,turn blue;
#define thresholdCenter (dryThreshold + wetThreshold)/2 //Brightest Green point
#define crossFade 20 //how much blue and red should fade in to green
#define PIN 8	 // pin Neopixel is attached to
#define NUMPIXELS 1 // number of neopixels in strip
#define SENSOR A1    //potentiometer acts as humidity indicator

//Pixel Indicator color selection values
int red = 0;
int green = 0;
int blue = 0;
//Humidity values
int sensorValue  = 0;
int transitionValue = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
void setup()
{
 pixels.begin();
 LCD.begin(16,2);       
 pinMode(SENSOR,INPUT);
 pinMode(heater_pin,OUTPUT);
 pinMode(fan_pin,OUTPUT);
 pinMode(switchPin,INPUT);
 Serial.begin(9600);
 startup();
 LCD.print("Room Temperature: ");
 delay(250); LCD.setCursor(2,1);
 LCD.print(mintemp);LCD.print("-");
 LCD.print(maxtemp);delay(1500);
}

void loop()
{  
  sensorValue = analogRead(SENSOR);
  transitionValue = map(sensorValue,0,1023,0,255);
  setColor();
  pixels.setPixelColor(0,red,green,blue);
  pixels.show();
  humidityDisplay();
  
  float eqvolt=analogRead(temp_trans_pin)*5.0/1023;
  float Sensortemp=100.0*eqvolt-50;
  int switch_state=digitalRead(switchPin);
  
  LCD.print("Temperature:");delay(500);
  LCD.setCursor(3,1);LCD.print(Sensortemp);
  LCD.print("C");
  delay(1000);LCD.clear();
  float d1=abs(maxtemp-Sensortemp);
  float d2=abs(Sensortemp-mintemp);
  if(mintemp<Sensortemp && maxtemp>Sensortemp){
    LCD.setCursor(2,0);LCD.print("  Temp is OK!");
    LCD.clear();delay(1000);
    LCD.setCursor(2,0);
    LCD.print("POWER SAVING");LCD.setCursor(3,1);
    LCD.print("  MODE");
    for(int i=0;i<3;i++){LCD.print(".");delay(250);}
    LCD.print(":)");
  }
  else if(switch_state==HIGH){
  if(Sensortemp>maxtemp){
    LCD.setCursor(0,0); LCD.print("HIGH Temp");
    delay(750);
    LCD.setCursor(0,1);LCD.print("  Turn on FAN");
    delay(1000);LCD.clear();
    LCD.setCursor(2,1);LCD.print("COOLING");
    for(int i=0;i<3;i++){LCD.print(".");delay(50*d1);}
    for(int i=0;i<256;i++)
    {analogWrite(fan_pin,i);}
    
    LCD.clear();LCD.setCursor(0,1);
    LCD.print("Turn OFF FAN");
    for(int i=255;i>=0;i--)
    {analogWrite(fan_pin,i);} 
  }
  else if(Sensortemp<mintemp){
    LCD.setCursor(5,1);LCD.print("LOW Temp ;)");
    delay(500);
    LCD.setCursor(0,0);LCD.print("Turn on");
    delay(500);
    LCD.setCursor(0,1);LCD.print("HEATER");
    LCD.clear(); 
    LCD.setCursor(2,1);LCD.print("Heating");
    for(int i=0;i<3;i++){LCD.print(".");delay(50*d2);}
    digitalWrite(heater_pin,HIGH);
    digitalWrite(heater_pin,LOW);LCD.clear();
    LCD.setCursor(2,0);LCD.command(0x18);
    LCD.print("Turning OFF"); delay(500);
    LCD.print("HEATER");
   } 
  else{
    LCD.setCursor(2,1);
    LCD.print("Something ");delay(500);
    LCD.print("went WRONG");
    for(int i=0;i<3;i++){LCD.print("!");delay(500);}
  }    
  delay(1000);LCD.clear();
  LCD.setCursor(2,1);
  LCD.print("Temp is OK!");
}  
else {
   LCD.command(0x18);LCD.setCursor(0,1);
   LCD.print("Turn on");delay(500);
   LCD.print("Temp controls");
}
  // turn off
  pixels.fill(0x000000);
  pixels.show();
  delay(500); // wait half a second
     delay(1000);   
}

void setColor(){
  red = ((transitionValue <= dryThreshold + crossFade) && (transitionValue >= 0 ))? 
    map(transitionValue,0,dryThreshold + crossFade,255,0) : 0;
  blue = (transitionValue >= wetThreshold - crossFade && transitionValue <= 255)? 
    map(transitionValue,wetThreshold - crossFade,255,0,255):0; 

  if(transitionValue >= dryThreshold && transitionValue <= thresholdCenter)
    green = map(transitionValue,dryThreshold,thresholdCenter,0,255);
  
  else if(transitionValue > thresholdCenter && transitionValue < wetThreshold)
    green = map(transitionValue,dryThreshold,thresholdCenter,255,0);

  else green = 0;                   
}

void startup(){
 LCD.clear();LCD.setCursor(2,0);
 LCD.print("HI!");delay(200);
 LCD.print("HEMANTH"); delay(500);
 LCD.setCursor(0,1);LCD.print("LOADING"); delay(500);
 for(int i=0;i<3;++i) {delay(500);LCD.print(".");}
 LCD.clear();
}

void humidityDisplay(){
  LCD.clear();
  LCD.print("HUMIDITY:");delay(500);
  int value = analogRead(A1);
  LCD.print(value);
  LCD.setCursor(0, 0);
  if (value <= 164) LCD.print("  Dry Weather   ");
  else if(value>800)LCD.print("     Very       ");
  LCD.setCursor(0, 1);
  
  if (value <= 164) LCD.print("    No Rain");
  else if ((value > 165) && (value <= 327)) 
    LCD.print("   Heavy Mist"); 
  else if ((value > 327) && (value <= 511))
    LCD.print("   Drizzle");
  else if ((value > 511) && (value <= 700)) 
    LCD.print("Average Rainfall");
  else if ((value > 700) && (value <= 800)) 
    LCD.print("   Heavy Rain");
  else    LCD.print("  Extreme Rain");

  delay(1000); LCD.clear();
}
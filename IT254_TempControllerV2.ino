bool dataGathered = false;
bool live = false;

float lowTemp;
float highTemp;
float tempRange;
int lowHumid;
int highHumid;
float humidRange;

float lastTemp;
float lastHumid;
int min = 60000;
int hr = 3600000;
float input;
int extNow;
int extHr;
float comparison = input / extNow;

#include <DHT.h>
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float humid = dht.readHumidity();
float temp = dht.readTemperature(true);


const char outHeat = 'A5';
const int outFan = 5;
const int outHumid = 7;
const int outDehumid = 9;
int sysDelay = 1000;
const float aref_voltage = 1.1;

void setup () {
	//Temperature and humidity are measured in analog values, and heater can be set to a specific value, humidifier, dehumidifier, and fan are digital since they are simply on or off
	Serial.begin(9600);
  dht.begin();
  pinMode(outHeat, OUTPUT);
  pinMode(outFan, OUTPUT);
  pinMode(outHumid, OUTPUT);
  pinMode(outDehumid, OUTPUT);
  analogReference(aref_voltage);
	
  Serial.println("Enter lowest acceptable temperature in Fehrenheit for your space (decimals and negative numbers are accepted):");
  delay(sysDelay);
  while (Serial.available() == 0){}
  lowTemp = Serial.parseFloat();

  Serial.println("Enter highest acceptable temperature in Fehrenheit for your space (decimals and negative numbers are accepted):");
  delay(sysDelay);
  while (Serial.available() == 0){}
  highTemp = Serial.parseFloat();
  delay(1000);

  tempRange = highTemp - lowTemp;

  Serial.println("Enter lowest acceptable humidity in a whole number without the percent sign for for your space:");
  delay(sysDelay);
  while (Serial.available() == 0){}
  lowHumid = Serial.parseInt();

  Serial.println("Enter highest acceptable humidity in a whole number without the percent sign for your space:");
  delay(sysDelay);
  while (Serial.available() == 0){}
  highHumid = Serial.parseInt();
  delay(1000);

  humidRange = highHumid - lowHumid;

  Serial.println("Would you like the system to run in live mode instead of predictive mode? (type \"Y\", \"YES\", or \"LIVE\")");
  delay(sysDelay);
  while (Serial.available() == 0){}
  String choice = String(Serial.read());
  delay(1000);
  choice.trim();
  choice.toLowerCase();
  if(choice.equals("y") || (choice.equals("yes")) || (choice.equals("live"))){
    live = true;
  }
  else{
    live = false;
  }
  delay(1000);
}

float tempDelta = 0;
float humiDelta = 0;
void gatherData(){
  while(true){
    Serial.println(temp);
    Serial.println(humid);
    lastTemp = temp;
    lastHumid = humid;
    
    delay(min);

    tempDelta = lastTemp - temp;
    humiDelta = lastHumid - humid;
  }
dataGathered = true;
}

float delta;
void loop(){
  
  if(!dataGathered){
    gatherData();
  }
  else{
    if(live){
      //Live mode
      //Temp control
      if(temp < lowTemp){
        analogWrite(outHeat, 200);
        digitalWrite(outFan, LOW);
      }
      else if(temp > highTemp){
        analogWrite(outHeat, 0);
        digitalWrite(outFan, HIGH);
      }
      else if(tempDelta <= -1){
        analogWrite(outHeat, 200);
        digitalWrite(outFan, LOW);
      }
      else if(tempDelta >= 1){
        analogWrite(outHeat, 0);
        digitalWrite(outFan, HIGH);
      }
      else{
        analogWrite(outHeat, 0);
        digitalWrite(outFan, LOW);
      }
      
      //Humidity Control
      if(temp < lowHumid){
        digitalWrite(outHumid, HIGH);
        digitalWrite(outDehumid, LOW);
      }
      else if(temp > highHumid){
        digitalWrite(outHumid, LOW);
        digitalWrite(outDehumid, HIGH);
      }
      else if(humiDelta <= -1){
        digitalWrite(outHumid, HIGH);
        digitalWrite(outDehumid, LOW);
      }
      else if(humiDelta >= 1){
        digitalWrite(outHumid, LOW);
        digitalWrite(outDehumid, HIGH);
      }
      else{
        digitalWrite(outHumid, LOW);
        digitalWrite(outDehumid, LOW);
      }
    }
    else{
      //Prediction mode

      //API Temp Calls
      input = temp;
      Serial.println("Current Temp");
      delay(3000);
      if(Serial.available() == 0){
      extNow = Serial.parseInt();
      }

      Serial.println("Future Temp");
      delay(3000);
      if(Serial.available() == 0){
      extHr = Serial.parseInt();
      delay(1000);
      }

      if((extHr * comparison) < lowTemp){
        while(temp < (highTemp - (0.1 * tempRange))){
          analogWrite(outHeat, 30);
          digitalWrite(outFan, LOW);
        }
      }
      else if((extHr * comparison) > highTemp){
        while(temp > (lowTemp + (0.1 * tempRange))){
          analogWrite(outHeat, 0);
          digitalWrite(outFan, HIGH);
        }
      }
      else{
        analogWrite(outHeat, 0);
        digitalWrite(outFan, LOW);
      }
      
      //API Humidity Calls
      input = humid;
      Serial.println("Current Humidity");
      delay(3000);
      if(Serial.available() == 0){
      extNow = Serial.parseInt();
      delay(1000);
      }

      Serial.println("Future Humidity");
      delay(3000);
      if(Serial.available() == 0){
      extHr = Serial.parseInt();
      }

      //Predictive Humidity Control
      if((extHr * comparison) < lowHumid){
        while(humid < (highHumid - (0.1 * humidRange))){
          digitalWrite(outHumid, HIGH);
          digitalWrite(outDehumid, LOW);
        }
      }
      else if((extHr * comparison) > highHumid){
        while(humid > (lowHumid + (0.1 * humidRange))){
          digitalWrite(outHumid, LOW);
          digitalWrite(outDehumid, HIGH);
        }
      }
      else{
        digitalWrite(outHumid, LOW);
        digitalWrite(outDehumid, LOW);
      }
      delay(1000);
    }

  }

} 
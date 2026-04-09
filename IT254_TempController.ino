bool dataGathered = false;
float lowTemp;
float highTemp;
float tempData[10];
float humiData[10];
int min = 60000;
int hr = 3600000;
#include <DHT.h>
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float humid = dht.readHumidity();
float temp = dht.readTemperature();


const char outHeat = 'A5';
const int outFan = 5;
int sysDelay = 1000;
const float aref_voltage = 1.1;

void setup () {
	//Temperature and humidity are measured in analog values, and heater can be set to a specific value, humidifier, dehumidifier, and fan are digital since they are simply on or off
	Serial.begin(9600);
  dht.begin();
  pinMode(outHeat, OUTPUT);
  pinMode(outFan, OUTPUT);
  analogReference(aref_voltage);
	
  Serial.println("Enter lowest acceptable temperature in Celsius for your space (decimals and negative numbers are accepted):");
  delay(sysDelay);
  while (Serial.available() == 0){}
  lowTemp = Serial.parseFloat();

  Serial.print("Enter highest acceptable temperature in Celsius for your space (decimals and negative numbers are accepted):");
  delay(sysDelay);
  while (Serial.available() == 0){}
  highTemp = Serial.parseFloat();
  delay(1000);
}
float tempDelta;
float humiDelta;
int events;
void gatherData(){
  events = 0;
  for(int i = 0; i < 10; i++){
	  tempData[i] = temp;
    Serial.println(temp);
    humiData[i] = humid;
    Serial.println(humid);
	  if(tempData[i] < lowTemp || tempData[i] > highTemp){
		  events++;
    }
    if(i < 0){
      tempDelta = (tempData[i-1]-tempData[i]);
      humiDelta = (humiData[i-1]-tempData[i]);
    }
  delay(min);
  }
dataGathered = true;
}
float delta;
void loop(){
  gatherData();
  if(!dataGathered){
    delay(min*10);
  }
  else{
    for(int k = 0; k < 9; k++){
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
    }

  }

} 
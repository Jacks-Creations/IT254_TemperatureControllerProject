//IT 254
#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float lowTemp = 0;
float highTemp = 0;
float tempRange = 0;

int lowHumid = 0;
int highHumid = 0;
float humidRange = 0;

bool live = true;
bool packetStart = false;
String input = "";

float temp = dht.readTemperature(true);
float humid = dht.readHumidity();

float lastTemp = 0;
float lastHumid = 0;

float tempCurrent = 0;
float tempAhead = 0;

float humidCurrent = 0;
float humidAhead = 0;

float tempDelta = 0;
float humiDelta = 0;

bool hasData = false;

const int outHeat = A5;
const int outFan = 5;
const int outHumid = 7;
const int outDehumid = 9;

int min = 60000;
int hr = 3600000;

int dataIndex = 0;
String message = "";
void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(outHeat, OUTPUT);
  pinMode(outFan, OUTPUT);
  pinMode(outHumid, OUTPUT);
  pinMode(outDehumid, OUTPUT);

  delay(5000);
  Serial.println("Hit enter to start");
  while (Serial.available() == 0) {}
  input = Serial.readStringUntil('\n');
  if(input.equals("p")){
    live = false;
  }

  Serial.println("Enter lowest acceptable temperature in Fehrenheit: ");
  while (Serial.available() == 0) {}
  input = Serial.readStringUntil('\n');
  lowTemp = input.toFloat();

  Serial.println("Enter highest acceptable temperature in Fehrenheit: ");
  while (Serial.available() == 0) {}
  input = Serial.readStringUntil('\n');
  highTemp = input.toFloat();

  Serial.println("Enter lowest acceptable humidity as whole number: ");
  while (Serial.available() == 0) {}
  input = Serial.readStringUntil('\n');
  lowHumid = input.toInt();

  Serial.println("Enter highest acceptable humidity as whole number: ");
  while (Serial.available() == 0) {}
  input = Serial.readStringUntil('\n');
  highHumid = input.toInt();

  tempRange = highTemp - lowTemp;
  humidRange = highHumid - lowHumid;

  Serial.println("READY");
}

void updateSensors() {
  lastTemp = temp;
  lastHumid = humid;
  temp = dht.readTemperature(true);
  humid = dht.readHumidity();
  tempDelta = temp - lastTemp;
  humiDelta = humid - lastHumid;
}

void loop() {
  updateSensors();
  if (live) {

    if (temp < lowTemp || tempDelta < -1) {
      analogWrite(outHeat, 200);
      digitalWrite(outFan, LOW);
      Serial.println("too cold");
    }
    else if (temp > highTemp || tempDelta > 1) {
      analogWrite(outHeat, 0);
      digitalWrite(outFan, HIGH);
      Serial.println("too hot");
    }
    else {
      analogWrite(outHeat, 0);
      digitalWrite(outFan, LOW);
      Serial.println("just right (temp)");
    }

    if (humid < lowHumid || humiDelta < -1) {
      digitalWrite(outHumid, HIGH);
      digitalWrite(outDehumid, LOW);
      Serial.println("too dry");
    }
    else if (humid > highHumid || humiDelta > 1) {
      digitalWrite(outHumid, LOW);
      digitalWrite(outDehumid, HIGH);
      Serial.println("too humid");
    }
    else {
      digitalWrite(outHumid, LOW);
      digitalWrite(outDehumid, LOW);
      Serial.println("just right(humid)");
    }
      delay(10000);
  }
  //predictive mode
  else {
    if (Serial.available() > 0) {
        message = Serial.readStringUntil('\n');
        message.trim();

        if (message.length() == 0) return;

          if (message == "START") {
            packetStart = true;
            dataIndex = 0;
            return;
          }
          if(!packetStart) return;

        if (message == "END") {
          if (dataIndex != 4) {
            packetStart = false;
            dataIndex = 0;
            return;
          }
          packetStart = false;
          if(!isfinite(tempCurrent) || !isfinite(humidCurrent) || tempCurrent == 0 || humidCurrent == 0) {
            packetStart = false;
            dataIndex = 0;
            return;
          }
          if(!isfinite(tempAhead) || !isfinite(humidAhead) || tempAhead == 0 || humidAhead == 0) {
            packetStart = false;
            dataIndex = 0;
            return;
          }
        float tempDeltaPre = tempAhead - tempCurrent;
        float humidDeltaPre = humidAhead - humidCurrent;
        
        float adjustedTemp = (temp / tempCurrent) * tempAhead;
        float adjustedHumid = (humid / humidCurrent) * humidAhead;

        if (adjustedTemp > highTemp || tempDeltaPre > tempRange) {
            analogWrite(outHeat, 0);
            digitalWrite(outFan, HIGH);
        }
        else if (adjustedTemp < lowTemp || tempDeltaPre < (-tempRange)) {
            analogWrite(outHeat, 200);
            digitalWrite(outFan, LOW);
        }
        else{
          analogWrite(outHeat, 0);
          digitalWrite(outFan, LOW);
        }

        if (adjustedHumid > highHumid || humidDeltaPre > humidRange) {
            digitalWrite(outHumid, LOW);
            digitalWrite(outDehumid, HIGH);
          }
        else if (adjustedHumid < lowHumid || humidDeltaPre < (-humidRange)) {
            digitalWrite(outHumid, HIGH);
            digitalWrite(outDehumid, LOW);
          }
        else{
          digitalWrite(outHumid, LOW);
          digitalWrite(outDehumid, LOW);
        }
        dataIndex = 0;
        return;
      }
      float value = message.toFloat();
        if (dataIndex == 0) tempCurrent = value;
        else if (dataIndex == 1) tempAhead = value;
        else if (dataIndex == 2) humidCurrent = value;
        else if (dataIndex == 3) humidAhead = value;
        dataIndex++;
      }
    }
  }
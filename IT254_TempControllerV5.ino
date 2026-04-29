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

bool live = false;
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
  highHumid = input.toInt();

  Serial.println("Enter highest acceptable humidity as whole number: ");
  while (Serial.available() == 0) {}
  input = Serial.readStringUntil('\n');
  lowHumid = input.toInt();

  tempRange = highTemp - lowTemp;
  humidRange = highHumid - lowHumid;

  Serial.println("Would you like to turn on Live mode? (Enter L.) Default is Predictive. (Enter anything else.): ");
  while (Serial.available() == 0) {}
  String mode = Serial.readStringUntil('\n');
  mode.trim();
  mode.toLowerCase();

  if (mode == "l") {
    live = true;
  }
  else {
    live = false;
  }
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
    Serial.println(temp);
    Serial.println(humid);

    if (temp < lowTemp || tempDelta < -1) {
      analogWrite(outHeat, 200);
      digitalWrite(outFan, LOW);
    }
    else if (temp > highTemp || tempDelta > 1) {
      analogWrite(outHeat, 0);
      digitalWrite(outFan, HIGH);
    }
    else {
      analogWrite(outHeat, 0);
      digitalWrite(outFan, LOW);
    }

    if (humid < lowHumid || humiDelta < -1) {
      digitalWrite(outHumid, HIGH);
      digitalWrite(outDehumid, LOW);
    }
    else if (humid > highHumid || humiDelta > 1) {
      digitalWrite(outHumid, LOW);
      digitalWrite(outDehumid, HIGH);
    }
    else {
      digitalWrite(outHumid, LOW);
      digitalWrite(outDehumid, LOW);
    }
      delay(10000);
  }
  //predictive mode
  else {
    if (Serial.available() > 0) {
      while (Serial.available() > 0) {
        char c = Serial.read();
        if (c != '\n') {
          message += c;
          continue;
        }
          message.trim();
          if (message.length() == 0) {
            message = "";
            continue;
          }
          if (message == "START") {
            packetStart = true;
            dataIndex = 0;
            message = "";
            continue;
          }
          if(!packetStart) {
            message = "";
            continue;
          }
        if (message == "END") {
          if (dataIndex != 4) {
            packetStart = false;
            dataIndex = 0;
            message = "";
            return;
          }
        packetStart = false;

        if (isnan(temp) || isnan(tempCurrent)) return;
        if (isnan(humid) || isnan(humidCurrent)) return;

        float tempDeltaPre = tempAhead - tempCurrent;
        float humidDeltaPre = humidAhead - humidCurrent;
        float adjustedTemp =  (temp / tempCurrent) * tempAhead;
        float adjustedHumid = (humid / humidCurrent) * humidAhead;

        int i = 0;
        if (adjustedTemp < lowTemp || tempDeltaPre < -(tempRange / 2)) {
          while((temp < highTemp) && i < 5){
            analogWrite(outHeat, 200);
            digitalWrite(outFan, LOW);
            i++;
            delay(3000);
            temp = dht.readTemperature(true);
          }
          if(temp >= highTemp){
            analogWrite(outHeat, 0);
          }
        }
        else if (adjustedTemp > highTemp || tempDeltaPre > (tempRange / 2)) {
          while((temp > lowTemp) && i < 5){
            analogWrite(outHeat, 0);
            digitalWrite(outFan, HIGH);
            i++;
            delay(3000);
            temp = dht.readTemperature(true);
          }
          if(temp <= lowTemp){
            digitalWrite(outFan, LOW);
          }
        }
        else{
          analogWrite(outHeat, 0);
          digitalWrite(outFan, LOW);
          delay(15000);
        }

        int j = 0;
        if (adjustedHumid < lowHumid || humidDeltaPre < -(humidRange / 2)) {
          while((humid < highHumid) && j < 5){
            digitalWrite(outHumid, HIGH);
            digitalWrite(outDehumid, LOW);
            j++;
            delay(3000);
            humid = dht.readHumidity();
          }
          if(humid >= highHumid){
            digitalWrite(outHumid, LOW);
          }
        }
        else if (adjustedHumid > highHumid || humidDeltaPre > (humidRange / 2)) {
          while((humid > lowHumid) && j < 5){
            digitalWrite(outHumid, LOW);
            digitalWrite(outDehumid, HIGH);
            j++;
            delay(3000);
            humid = dht.readHumidity();
          }
          if(humid <= lowHumid){
            digitalWrite(outDehumid, LOW);
          }
        }
        else{
          digitalWrite(outHumid, LOW);
          digitalWrite(outDehumid, LOW);
        }
        dataIndex = 0;
        message = "";
        return;
      }
      float value = message.toFloat();
        if (dataIndex == 0) tempCurrent = value;
        else if (dataIndex == 1) tempAhead = value;
        else if (dataIndex == 2) humidCurrent = value;
        else if (dataIndex == 3) humidAhead = value;
        dataIndex++;
        message = "";
      }
    }
  }
}
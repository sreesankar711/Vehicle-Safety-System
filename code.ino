#include <LiquidCrystal.h>
#include <DHT.h>
#include <TinyGPS.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

float h;
float t;

const int DHTPIN = 12;
const int buzzer_Pin = 3;
const int ir_Sensor = 10;
const int vibration_Sensor = 2;
const int echoPin = 11; 
const int pingPin = 13; 
const int alertDistance = 50; 

long duration, cm;


TinyGPS gps;
long lat, lon;
bool ir_status = LOW;
bool vibration_Status = LOW;

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void seatbeltTask(); 

void setup() {
  Serial.begin(9600);
  pinMode(buzzer_Pin, OUTPUT);
  pinMode(ir_Sensor, INPUT);
  pinMode(vibration_Sensor, INPUT);
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);


  lcd.begin(16, 2);
  lcd.print("CAR MANAGEMENT");
  lcd.setCursor(4, 2);
  lcd.print("SYSTEM");
  dht.begin();



  delay(250);
}

void loop() {
  lcd.clear();
  h = dht.readHumidity();
  delay(50);
  t = dht.readTemperature();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(h);
  lcd.print(" %");

  delay(50);

  vibration_Status = digitalRead(vibration_Sensor);
  if (vibration_Status == HIGH) {
    accidentTask();
  }

  cm = read_dist(duration);

  while (cm < alertDistance) {
    distanceTask();
    cm = read_dist(duration);
  }

  ir_status = digitalRead(ir_Sensor);
  while (ir_status == LOW) {
    seatbeltTask();
    ir_status = digitalRead(ir_Sensor);
  }

  delay(50);

  digitalWrite(buzzer_Pin, LOW);
}

void accidentTask() {
  lcd.clear();
  lcd.print("Accident Detected");
  lcd.setCursor(3, 2);
  lcd.print("Sending Msg");
  delay(500);
  Serial.println("AT+CMGF=1");
  delay(100);
  Serial.println("AT+CMGS=\"+911234567890\"\r");
  delay(100);
  Serial.println("Accident Detected");
  Serial.println("please check location");
  int i=1;
  while(i>0)
  {
    byte a;

  if (Serial.available()) {
    a = Serial.read();
    while (gps.encode(a)) {
      gps.get_position(&lat, &lon);
      i=i-1;
      
    }
  }

  }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lat: ");
      lcd.print(lat * 0.000001, 8);
      lcd.setCursor(0, 1);
      lcd.print("Lon: ");
      lcd.print(lon * 0.000001, 8);

      
      Serial.println("Lat: ");
      Serial.println(lat * 0.000001, 8);
      Serial.println("Lon: ");
      Serial.println(lon * 0.000001, 8);
  
  delay(100);
}




void distanceTask() {
  vibration_Status = digitalRead(vibration_Sensor);
  if (vibration_Status == HIGH) {
    accidentTask();
  }

  digitalWrite(buzzer_Pin, HIGH);
  lcd.clear();
  lcd.print("ALERT: Distance");
  lcd.setCursor(3, 2);
  lcd.print("< 50cm");
  delay(100);
}


void seatbeltTask() {
  vibration_Status = digitalRead(vibration_Sensor);
  if (vibration_Status == HIGH) {
    accidentTask();
  }

  lcd.clear();
  lcd.print("Seat Belt");
  lcd.setCursor(3, 2);
  lcd.print("not Detected");
  digitalWrite(buzzer_Pin, HIGH);
  delay(100);
}



long read_dist(long microseconds) {
  digitalWrite(pingPin, LOW);
  delayMicroseconds(1);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  return duration / 29 / 2;
}

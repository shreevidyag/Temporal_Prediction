#define DHTTYPE DHT11   // DHT 11
#define DHTPIN 2
#define DEBUG true
#include <DHT.h>
#include <DHT_U.h>

#include <ArduinoJson.h>
//#include <Ethernet.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//#include <SoftwareSerial.h>       
//SoftwareSerial espSerial(2, 3);   //#(RX,TX)

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

int pin10 = 3;
int pin25 = 6;
unsigned long duration1;
unsigned long duration2;

unsigned long starttime1;
unsigned long starttime2;
unsigned long sampletime_ms = 3000;//sample 1s ;
unsigned long lowpulseoccupancy1 = 0;
unsigned long lowpulseoccupancy2 = 0;
float ratio1 = 0;
float ratio2 = 0;
float concentration10 = 0;
float concentration25 = 0;

String mySSID = "CIA_VAN_23";     //Wi-Fi Access point
String myPWD = "jauyUug/aa";
//String mqtt_server = "localhost";

void callback(char* topic, byte* payload, unsigned int length) 
{
}

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);
  
  espData("AT+RST", 1000, DEBUG);                      
  espData("AT+CWMODE=1", 1000, DEBUG);                 
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);
  
  pinMode(pin10,INPUT);
  pinMode(pin25,INPUT);
  //dht.setup(2);
  starttime1 = millis();//get the current time;
  starttime2 = millis();//get the current time;

  client.setServer("localhost", 1883); // change port number as mentioned in your cloudmqtt console
  client.setCallback(callback);
}

float getPM25()
{
  duration1 = pulseIn(pin10, LOW);
  lowpulseoccupancy1 = lowpulseoccupancy1 + duration1;
  while ((millis()-starttime1) < sampletime_ms)//if the sampel time == 30s
  {
    ratio1 = lowpulseoccupancy1 / (sampletime_ms * 10.0);  // Integer percentage 0=>100
    concentration25 = 1.1*pow(ratio1,3)-3.8*pow(ratio1,2)+520*ratio1+0.62; // using spec sheet curve
    lowpulseoccupancy1 = 0;
    starttime1 = millis();
    return concentration25;
  }
}

float getPM10()
{
  duration2 = pulseIn(pin10, LOW);
  lowpulseoccupancy2 = lowpulseoccupancy2 + duration2;
  while ((millis()-starttime2) < sampletime_ms)//if the sampel time == 30s
  {
    ratio2 = lowpulseoccupancy2 / (sampletime_ms * 10.0);  // Integer percentage 0=>100
    concentration10 = 1.1*pow(ratio1,3)-3.8*pow(ratio1,2)+520*ratio1+0.62; // using spec sheet curve
    lowpulseoccupancy2 = 0;
    starttime2 = millis();
    return concentration10;
  }
}

float getHumidity()
{
  delay(2000);  /* Delay of amount equal to sampling period */
  float humidity = dht.readHumidity();     /* Get humidity value */
  float temperature = dht.readTemperature(); /* Get temperature value */
}

float getGasConcentration()
{
  int sensorValue = analogRead(A0);
}

void loop() {
  float pm10 = getPM10();
  float pm25 = getPM25();
  float rh = getHumidity();
  float gasLevel = getGasConcentration();

  JSonDocument doc;
  doc["pm10"] = pm10;
  doc["pm25"] = pm25;
  doc["rh"] = rh;
  doc["gasLevel"] = gasLevel;
  serializeJson(doc, Serial);
  
  String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD +"="+String(doc);
  espData("AT+CIPMUX=1", 1000, DEBUG);       
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
  espSerial.find(">"); 
  espSerial.println(sendData);
   
  espData("AT+CIPCLOSE=0",1000,DEBUG);
}

float calcAQI(float I_high, float I_low, float C_high, float C_low, float C) {
  return (I_high - I_low) * (C - C_low) / (C_high - C_low) + I_low;
}

int getAQI(int sensor, float density) {
  int d10 = density * 10;
  if ( sensor == 0 ) {
    if (d10 <= 0) {
      return 0;
    }
    else if(d10 <= 120) {
      return calcAQI(50, 0, 120, 0, d10);
    }
    else if (d10 <= 354) {
      return calcAQI(100, 51, 354, 121, d10);
    }
    else if (d10 <= 554) {
      return calcAQI(150, 101, 554, 355, d10);
    }
    else if (d10 <= 1504) {
      return calcAQI(200, 151, 1504, 555, d10);
    }
    else if (d10 <= 2504) {
      return calcAQI(300, 201, 2504, 1505, d10);
    }
    else if (d10 <= 3504) {
      return calcAQI(400, 301, 3504, 2505, d10);
    }
    else if (d10 <= 5004) {
      return calcAQI(500, 401, 5004, 3505, d10);
    }
    else if (d10 <= 10000) {
      return calcAQI(1000, 501, 10000, 5005, d10);
    }
    else {
      return 1001;
    }
  } else {
    if (d10 <= 0) {
      return 0;
    }
    else if(d10 <= 540) {
      return calcAQI(50, 0, 540, 0, d10);
    }
    else if (d10 <= 1540) {
      return calcAQI(100, 51, 1540, 541, d10);
    }
    else if (d10 <= 2540) {
      return calcAQI(150, 101, 2540, 1541, d10);
    }
    else if (d10 <= 3550) {
      return calcAQI(200, 151, 3550, 2541, d10);
    }
    else if (d10 <= 4250) {
      return calcAQI(300, 201, 4250, 3551, d10);
    }
    else if (d10 <= 5050) {
      return calcAQI(400, 301, 5050, 4251, d10);
    }
    else if (d10 <= 6050) {
      return calcAQI(500, 401, 6050, 5051, d10);
    }
    else {
      return 1001;
    }
  }   
}

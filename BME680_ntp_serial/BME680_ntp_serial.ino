
#include <Wire.h>
#include "bsec.h"
#include<string.h>  
#include "time.h"
#include <CayenneMQTTESP32.h>
#define CAYENNE_PRINT Serial


#define SEALEVELPRESSURE_HPA (1013.25)
//#define CAYENNE_PRINT Serial
#define LED_BUILTIN 2

char ssid[] = "Tevatron_2.4G";
char wifiPassword[] = "Teva@123#";

const char* ntpServer = "pool.ntp.org";
// set Indian time zone
const long  gmtOffset_sec = 3600*5+1800;
const int   daylightOffset_sec = 3600;
const int BUFFER_SIZE = 50;
char buf[BUFFER_SIZE];
char ntp_data[50];
uint8_t time_value[20];
// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.:ankita

char username[] = "6284c140-75f8-11ed-b193-d9789b2af62b";
char password[] = "0df68da8296be41bc65a47a505296aaf45952ae6";
char clientID[] = "5c602390-7602-11ed-b193-d9789b2af62b";

 
Bsec iaqSensor;
String output;
// Variables
float temperature;
float humidity;
float pressure;
float IAQ;
float carbon;
float VOC;
const char* IAQsts;
struct tm timeinfo;
 
void printLocalTime()
{
   if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      return;
   }
// serial print of date and time getting from NTP  
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    
// Serial monitor data read , to set time:ankita   
    if (Serial.available() > 0) {
    Serial.readBytes(buf, BUFFER_SIZE);
     }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
 
  Serial.println("Connecting to ");
   
  iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
 
  Serial.println(output);
  checkIaqSensorStatus();
  //Enumeration for output (virtual) sensors
  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
 
  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();
  // Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  
  }
 
void loop() {
  
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available:ankita
    output = String(time_trigger);
    output += ", " + String(iaqSensor.rawTemperature);
    output += ", " + String(iaqSensor.pressure);
    output += ", " + String(iaqSensor.rawHumidity);
    output += ", " + String(iaqSensor.gasResistance);
    output += ", " + String(iaqSensor.iaq);
    output += ", " + String(iaqSensor.iaqAccuracy);
    output += ", " + String(iaqSensor.temperature);
    output += ", " + String(iaqSensor.humidity);
    output += ", " + String(iaqSensor.staticIaq);
    output += ", " + String(iaqSensor.co2Equivalent);
    output += ", " + String(iaqSensor.breathVocEquivalent);
    Serial.println(output);
  } else {
    checkIaqSensorStatus();
  }
  delay(1000);
  printLocalTime();
  delay(1000);    
  Serial.print("Temperature = "); 
  Serial.print(iaqSensor.temperature); 
  Serial.println(" *C");
  delay(100);   
  
  Serial.print("Pressure = "); 
  Serial.print(iaqSensor.pressure / 100.0); 
  Serial.println(" hPa");
  delay(100);
     
  Serial.print("Humidity = "); 
  Serial.print(iaqSensor.humidity); 
  Serial.println(" %");
  delay(100);
   
  Serial.print("IAQ = "); 
  Serial.print(iaqSensor.staticIaq); 
  Serial.println(" PPM");
  delay(100); 
  
  Serial.print("CO2 equiv = "); 
  Serial.print(iaqSensor.co2Equivalent); 
  Serial.println(" PPM");
  delay(100); 
 
  Serial.print("Breath VOC = "); 
  Serial.print(iaqSensor.breathVocEquivalent); 
  Serial.println(" PPM");
  delay(100); 

  if ((iaqSensor.staticIaq > 0)  && (iaqSensor.staticIaq  <= 50)) {
    IAQsts = "Good";
    Serial.print("IAQ: Good"); 
    delay(1000);     
  }
  if ((iaqSensor.staticIaq > 51)  && (iaqSensor.staticIaq  <= 100)) {
    IAQsts = "Average";
    Serial.print("IAQ: Average");
    delay(1000); 
  }
  if ((iaqSensor.staticIaq > 101)  && (iaqSensor.staticIaq  <= 150)) {
    IAQsts = "Little Bad";
    Serial.print("IAQ: Little Bad");
    delay(1000);     
  }
  if ((iaqSensor.staticIaq > 151)  && (iaqSensor.staticIaq  <= 200)) {
    IAQsts = "Bad";
    Serial.print("IAQ: Bad");
    delay(1000); 
    
  }
  if ((iaqSensor.staticIaq > 201)  && (iaqSensor.staticIaq  <= 300)) {
    IAQsts = "Worse";
    Serial.print("IAQ: Worse");
  }
  if ((iaqSensor.staticIaq > 301)  && (iaqSensor.staticIaq  <= 500)) {
    IAQsts = "Very Bad";
    Serial.print("IAQ: Very Bad");
  }
  if ((iaqSensor.staticIaq > 500)){
  IAQsts = "Very Very Bad";
  Serial.print("IAQ: Very Very Bad");
  }
  Serial.println();
  // ntp_time store in buffer
  strftime(ntp_data,50,"%H:%M", &timeinfo);

   // data of sensor sent on cayenne server 
  if((strcmp(buf,ntp_data))==10)
  {
    Serial.println("Sending on server");    
    Cayenne.loop();
    Cayenne.celsiusWrite(1,iaqSensor.temperature); 
    Cayenne.hectoPascalWrite(2,iaqSensor.pressure / 100.0);
    Cayenne.virtualWrite(3,iaqSensor.humidity,"hum","%"); 
    Cayenne.virtualWrite(4,iaqSensor.staticIaq,"IAQ","ppm");
    Cayenne.virtualWrite(5,iaqSensor.co2Equivalent,"CO2 equiv","ppm");
    Cayenne.virtualWrite(6,iaqSensor.breathVocEquivalent,"Breath VOC","ppm");
  }
}
// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }
 
  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}



void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
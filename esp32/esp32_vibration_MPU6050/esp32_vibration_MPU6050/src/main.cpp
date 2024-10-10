#include <Arduino.h>
#include<Wire.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#include <string.h>
const char* ssid = "huu nam";
const char* password = "matkhau987" ;

#define VBR 5

#define I2C_SDA 16
#define I2C_SCL 17

#define ACCEL_XOUTH 0x3B
#define ACCEL_XOUTL 0x3C
#define ACCEL_YOUTH 0x3D
#define ACCEL_YOUTL 0x3E
#define ACCEL_ZOUTH 0x3F
#define ACCEL_ZOUTL 0x40
#define GYRO_XH 0x43
#define GYRO_XL 0x44
const int MPU = 0x68;
float AccX,AccY,AccZ;
float timer;
WiFiClient client;
unsigned long myChannelNumber = 1;
const char * myWriteAPIkey = "RI6JRFIU1ORDNK4X";

long vibration()
{
  long measurement = pulseIn(VBR,HIGH);
  return measurement;
}

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  pinMode(VBR, INPUT);
  Serial.begin(115200);
  
    //setup wifi
  WiFi.begin(ssid,password);
  Serial.println("conecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);

}
 
void loop() {
  
  // Wait a few seconds between measurements.
  delay(300);
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);
  AccX =(float)(Wire.read() << 8 |Wire.read())/16384.0;
  AccY =(float)(Wire.read() << 8 |Wire.read())/16384.0;
  AccZ =(float)(Wire.read() << 8 |Wire.read())/16384.0;
  long measurement = vibration();
  Serial.println(measurement);
  Serial.print(AccX);
  Serial.print(",");
  Serial.print(AccY);
  Serial.print(",");
  Serial.println(AccZ);
  
  ThingSpeak.setField(1,AccX);
  ThingSpeak.setField(2,AccY);
  ThingSpeak.setField(3,AccZ);
  
  ThingSpeak.setField(4,measurement);
  int x = ThingSpeak.writeFields(myChannelNumber,myWriteAPIkey);
}
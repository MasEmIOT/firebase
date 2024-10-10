#include <Arduino.h>
#include<Wire.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <WiFi.h>
#include <string.h>
const char* ssid = "huu nam";
const char* password = "matkhau987" ;

#define API_KEY "AIzaSyAWwu7V7kSF-xqD5ME2njwH9MncBR80du8"
//#define FIREBASE_PROJECT_ID "denso-392b8"
#define DATABASE_URL "https://densofirebase-6356b-default-rtdb.asia-southeast1.firebasedatabase.app/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
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
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
 
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n",config.signer.signupError.message.c_str());
  }
  
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
 
void loop() {
  // Wait a few seconds between measurements.
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);
  AccX =(float)(Wire.read() << 8 |Wire.read())/16384.0;
  AccY =(float)(Wire.read() << 8 |Wire.read())/16384.0;
  AccZ =(float)(Wire.read() << 8 |Wire.read())/16384.0;
  long measurement = vibration();
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    if(Firebase.RTDB.setFloat(&fbdo,"/Vibra",measurement)){
      Serial.println(measurement);
    }
    if(Firebase.RTDB.setFloat(&fbdo,"/MPU6050/AccX",AccX)){
      Serial.print(AccX);
      Serial.print(","); 
    }
    if(Firebase.RTDB.setFloat(&fbdo,"/MPU6050/AccY",AccY)){
      Serial.print(AccY);
      Serial.print(","); 
    }
    if(Firebase.RTDB.setFloat(&fbdo,"/MPU6050/AccZ",AccZ)){
      Serial.println(AccZ);
    }
  }
}
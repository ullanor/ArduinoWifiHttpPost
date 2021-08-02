//#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <LPS.h>

#define LM35 A5 //analogTemp
#define led_r 5 //RGB_RED
#define led_g 6 //RGB_GREEN
#define led_b 3 //RGB_BLUE

LPS ps;
float temp = 0;
float pressure = 0;
float altitude = 0;
float temperature = 0;

//wifi setup
char ssid[] = "xxx";        // your network SSID (name)
char pass[] = "xxx";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;
char server[] = "192.168.0.250";
int wifiAttempts = 0;
//http post setup
String postData;
String postVar0 = "press=";
String postVar1 = "&temp=";
String postVar2 = "&tempA=";
bool infoSend = false;

WiFiClient client;

//-------------------------------------------------------
void setup() {
  Serial.begin(9600);  
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  Wire.begin();

  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }

  ps.enableDefault();

  _MainOperation();
}
 
void loop() {
  if((millis() / 1000)%1000 == 0){
    if(infoSend == false){
      _MainOperation();
      infoSend = true;
    }
  }else infoSend = false;
}

//----------------------------------- WIFI ------------------------------
void TryPostData(){
  postData = postVar0 + pressure + postVar1 + temperature + postVar2 + temp;
  if(client.connect(server, 80)){
    Serial.println("Connected to server");   
    client.println("POST /FILES/ArduinoDataUpload.php HTTP/1.1");
    client.println("Host: 192.168.0.250");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  }
  if(client.connected()){
    client.stop();
  }
  Serial.println(postData);
  Serial.println();
}
bool TryWifiConn(){
  status = WiFi.status();
  Serial.print("NEW CHECK Current WiFi status: ");
  Serial.println(status);

  if(status != WL_CONNECTED){
    RedRGB();
    wifiAttempts = 0;
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (wifiAttempts < 4){
      wifiAttempts += 1;
      Serial.print(wifiAttempts);
      Serial.println(" connection attempt");
      status = WiFi.begin(ssid, pass);
      delay(5000);//10000
      if(status == WL_CONNECTED)break;
    }
  }
  
  if(status == WL_CONNECTED){
    Serial.println("You're connected to the network");
    return true;
  }
  else{
    Serial.println("Cannot connect to the network");
    return false;
  }
}

//-------------------------------- DATA ---------------------------------
void _MainOperation(){
  if(!TryWifiConn()) return;
  Serial.print("Seconds: ");
  Serial.println(millis() / 1000);
  GreenRGB();
  Get_All_Data();
  BlueRGB();
}
void Get_All_Data(){
  Get_AnalogTemp_Data();
  Get_LPS_Data();
  TryPostData();
}
void Get_AnalogTemp_Data(){
  temp = ((analogRead(LM35) * 5.0) / 1024.0) * 100;
  Serial.print("Actual analog temp: ");
  Serial.print(temp);
  Serial.println("*C");
}
void Get_LPS_Data(){
  pressure = ps.readPressureMillibars();
  altitude = ps.pressureToAltitudeMeters(pressure);
  temperature = ps.readTemperatureC();
  
  Serial.print("p: ");
  Serial.print(pressure);
  Serial.print(" mbar\ta: ");
  Serial.print(altitude);
  Serial.print(" m\tt: ");
  Serial.print(temperature);
  Serial.println(" deg C");
}

void RedRGB(){
  analogWrite(led_r, 255);
  analogWrite(led_g, 0);
  analogWrite(led_b, 0);
}
void GreenRGB(){
  analogWrite(led_r, 255);
  analogWrite(led_g, 255);//analogWrite(led_g, 255);
  analogWrite(led_b, 255);
}
void BlueRGB(){
  analogWrite(led_r, 250);
  analogWrite(led_g, 255);
  analogWrite(led_b, 255);//analogWrite(led_b, 255);
}

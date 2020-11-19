#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

SoftwareSerial komunikasi (D6,D5);


const char* ssid = "696969";
const char* password = "00998877665544332211";

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.100.21/api/mc/config";
String serverLogin = "http://192.168.100.21/api/mc/auth";
String serverstorPH = "http://192.168.100.21/api/mc/monitor/fishpond/2";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.



//Parsing Serial
String sData, data[10];
boolean parsing = false;

//Device key Kontroler
const char* device_key = "device_key=15954747772";



String baca_eeprom_token(){
  //Serial.println("Reading EEPROM Token");
  String esid;
  for (int i = 0; i < 323; ++i)
    {
      esid += char(EEPROM.read(i));
    }
   //Serial.println(esid);
    //esid.trim();
  return esid;
  
}


String login(){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverLogin);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data yang dikirim lewat body
      String httpRequestData = device_key;           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString();
      String Token = payload;
      
      Serial.print("HTTP Response code Dari Server: ");
      Serial.println(httpResponseCode);
      Serial.print("Balasan String Dari Server Monggo : ");
      Serial.println(payload);

      //Parsing Data Json
      char json[500];
      payload.toCharArray(json, 500);
      StaticJsonDocument<200> doc;
      deserializeJson(doc, json);
      

      //Mengambil Value Key Json
      const char* message = doc["message"];
      
      //Lihat Hasil Parsing Json
      Serial.print("Hasil Parsing Json Borr");
      Serial.println(message);
      delay(5000);
      //Input Token Hasil Request Ke Dala EEPROM
      String qsid=message;
      int charLength=qsid.length();
 
      Serial.println("writing eeprom Token:");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
            }

      http.end();
      
}

String get_config(){
      String token_key = baca_eeprom_token();
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Authorization", "Bearer "+token_key);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          
      // Send HTTP POST request
      int httpResponseCode = http.GET();
      String payload = http.getString();
      String Token = payload;
      
      //Serial.print("HTTP Response code Dari Server: ");
      //Serial.println(httpResponseCode);
      //Serial.print("Balasan String Dari Server Monggo : ");
      //Serial.println(payload);
      //Parsing Data Json
      char json[700];
      payload.toCharArray(json, 700);
      StaticJsonDocument<300> doc;
      deserializeJson(doc, json);
      
      //Mengambil Value Key Json
      String ph_min = doc["message"][0]["config"]["conf_ph_min"];
      String ph_max = doc["message"][0]["config"]["conf_ph_max"];
      //Serial.print("PH Minimum : ");
      Serial.print(ph_min); Serial.print(","); Serial.println(ph_max);


     // Serial.print("PH Maximum : ");
     // Serial.println(ph_max);
      komunikasi.print("#"+ph_min+"#"+ph_max+"#$"); 
      http.end();
      return payload;
     
      
}

String setore_ph(){

  while (komunikasi.available()) {
    char inChar = komunikasi.read();
    //inChar.trim();
    sData += inChar;
    
    //  Serial.println("Data Masuk :" + sData);
      Serial.println("PH Kolam :" + sData);
      
      String token_key = baca_eeprom_token();
      const char* fishpond_id = "fishpond_id=2";
      String fspnd_ph = "{\"fishpond_id\": 1,\"fspnd_ph\": 2}";

      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverstorPH);

      // Specify content-type header
       http.addHeader("Authorization", "Bearer "+token_key);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     
      // Data yang dikirim lewat body
//      String httpRequestData = fishpond_id;
           
      // Send HTTP PUT request
      String dtToSend = "fspnd_ph=" + sData +"&fishpond_id=1";
      int httpResponseCode = http.sendRequest("PUT", dtToSend);
      String payload = http.getString();
      String Token = payload;
      
      Serial.print("HTTP Response code Dari Server: ");
      Serial.println(httpResponseCode);
      ;
      Serial.print("Balasan String Dari Server Monggo : ");
      Serial.println(payload);
      http.end();
      delay(1000);
      sData = "";
  }
  }




void setup() {
  Serial.begin(115200);
  komunikasi.begin(9500); 
  EEPROM.begin(512);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

  login();
  
}


unsigned long interval=2000; // the time we need to wait
unsigned long previousMillis=0; // millis() returns an unsigned long.
  
bool ledState = false;

void loop() {
  
 unsigned long currentMillis = millis(); // grab current time
  
 // check if "interval" time has passed (1000 milliseconds)
 if ((unsigned long)(currentMillis - previousMillis) >= interval) {
   
   ledState = !ledState; // "toggles" the state
   get_config();
   // sets the LED based on ledState
   // save the "current" time
   previousMillis = millis();
 }

 if ((unsigned long)(currentMillis - previousMillis) >= interval) {
   
   ledState = !ledState; // "toggles" the state
 //setore_ph();
   // sets the LED based on ledState
   // save the "current" time
   previousMillis = millis();
 }
 
  //baca_eeprom_token();
 
 
 //komunikasi.print(8,5);

}

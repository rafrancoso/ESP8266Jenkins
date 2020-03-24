#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

byte prod1 = D3;
byte prod2 = D4;

byte activeProd = 0;


ESP8266WiFiMulti WiFiMulti;

bool prodActive(){
  bool resultado = false;
  int WifiStatus = WiFiMulti.run();
  USE_SERIAL.println("prodActive: "+WifiStatus);
  if((WifiStatus == WL_CONNECTED)) {
    HTTPClient http;
    http.begin("ENVIRONMENT_JSON_URL_HERE");
    int httpCode = http.GET();
    USE_SERIAL.println("httpCode: "+httpCode);
    if(httpCode == HTTP_CODE_OK) {
        resultado = true;
        String payload = http.getString();
        int vactiveProd = payload.charAt(3);
        USE_SERIAL.println("xxxxxxxx");
        USE_SERIAL.println(activeProd);
        USE_SERIAL.println("xxxxxxxx");
        if (vactiveProd == 49){
          activeProd = prod1;
        }else{
          if (vactiveProd ==50){
            activeProd = prod2;
          }else{
            USE_SERIAL.println("PROD NÃO DETECTADO: "+payload);
          }
        }
        USE_SERIAL.println(payload);
    }
    http.end();
  }
  return resultado;
}
void callJenkins(String environment){
  HTTPClient http;
  http.begin("JENKINS_BUILD_URL_HERE/build?delay=0sec");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Basic YW55bWFya2V0OiFAI011ZGFy");
  http.POST("json={\"parameter\": {\"name\": \"TARGET\", \"value\": \""+environment+"\"}, \"statusCode\": \"303\", \"redirectTo\": \".\"}");
  http.writeToStream(&Serial);
  http.end();
}
void setup() {
    pinMode(prod1,INPUT_PULLUP);
    pinMode(prod2,INPUT_PULLUP);
    
    USE_SERIAL.begin(115200);
    //USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
    
    WiFiMulti.addAP("sid", "password");
    delay(2000);
    while (!prodActive()){
      USE_SERIAL.println("Nao conectou, tentar novamente em 10s");
      delay(10000);  
    }
    USE_SERIAL.println("end setup");    
}

void loop() {    
    if (digitalRead(prod1)==LOW){
      if (activeProd != prod1){      
        Serial.println("Prod1 Active");
        activeProd = prod1;
        callJenkins("runInProd");
      }
    }else{
      if (digitalRead(prod2)==LOW){
        if (activeProd != prod2){
          Serial.println("Prod2 Active");  
          activeProd = prod2;
          callJenkins("runInProd2");
        }
      }      
    }

  delay(500);
}

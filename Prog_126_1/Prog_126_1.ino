
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ArduinoOTA.h>
#include <PubSubClient.h>


const char* ssid = "Fibertel WiFi469";
const char* password = "passwd";

//---------- mqtt
const char* mqtt_server = "tailor.cloudmqtt.com";
int serverport = 12337;
const char* usernameMqtt = "tico";
const char* passwordMqtt = "passwd";
const char* topicBombaLavadero = "/lavadero/bomba";
const char* topicBombaLavaderoEspOK = "/lavadero/bomba/espOK";

WiFiClient espClient;
PubSubClient client(espClient);
//---------- mqtt

IPAddress ip(192,168,0,200);     
IPAddress gateway(192,168,0,1);   
IPAddress subnet(255,255,255,0);  

ESP8266WebServer server(80);

int led = 2;

unsigned long lastMsg = 0;

void prenderBomba(){
  digitalWrite(led, LOW);
}


void apagarBomba(){
  digitalWrite(led, HIGH);
}

void handleOn(){
  String message ="on";
  prenderBomba();
  server.send(200, "text/plain", message);
}

void handleOff(){
  String message ="off";
  apagarBomba();
  server.send(200, "text/plain", message);
}


void handleRoot() {
  const char * htmlMessage = "<!DOCTYPE html>\
  <html>\
  <head>\
    <title>ESP8266 Demo</title>\
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      button{ width: 250px; max-width: 100%;padding: 15px; font-size: 30px; font-weight: bold;}\
    </style>\
    <script>\
      let prendido=false;\
      let http_request=false;\
      let startTime, endTime;\
      function prender(){\
        startTime = new Date();\
        document.getElementById('error').innerHTML ='';\
        document.getElementById('tiempo').innerHTML ='';\
        document.getElementById('prenderApagar').disabled=true;\
        console.log(prendido);\
        console.log(window.location);\
        http_request = new XMLHttpRequest();\
        http_request.onreadystatechange = callBack;\
        let url = '';\
        if(prendido){\
          document.getElementById('prenderApagar').innerHTML = 'Prender';\
          url= url + '/off';\
        }else{\
          url= url + '/on';\
          document.getElementById('prenderApagar').innerHTML = 'Apagar';\
        }\
        console.log(url);\
        http_request.open('GET', url, true);\
        http_request.send();\
        prendido=!prendido;\
      }\
      function callBack(){\
        console.log('callBack');\
         if (http_request.readyState == 4) {\
            if (http_request.status == 200) {\
                document.getElementById('prenderApagar').disabled=false;\
                endTime = new Date();\
                let timeDiff = endTime - startTime;\
                let seconds = Math.round(timeDiff/1000);\
                document.getElementById('tiempo').innerHTML = 'tardo ' + seconds + ' segundos en realizarse (' + http_request.responseText + ')';\
            } else {\
                document.getElementById('error').innerHTML ='Hubo problemas con la petición.';\
            }\
         }\
      }\
    </script>\
  </head>\
  <body>\
    <h1>Bomba ESP8266!</h1>\
    <span><button id='prenderApagar' type='button' onclick=\"prender();\">Prender</button></span>\
    <div id='tiempo'></div>\
    <div id='error'></div>\
  </body>\
</html>";

  server.send(200, "text/html", htmlMessage);
}

void handleNotFound() {
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
}

void drawGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void handleMqtt(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();
  
  if (String(topic) == String(topicBombaLavadero)){
    if ((char)payload[0] == '1') {
      Serial.println("activado");
      prenderBomba();
      }
    if ((char)payload[0] == '0') {
      Serial.println("desactivado");
      apagarBomba();
      }
  }//topicBombaLavadero

  //aviso que está conectado al iniciar la app y en cada ejecucion de un pedido mqtt
  publicMqttEspAlive();
  
  
}


void conectarAWifiYOtaSetup(){
  Serial.begin(9600);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  //WiFi.hostname("esp8266-tanque");
  /*
   * 
   * OJO!! Con IP fija no me anduvo la conexion al MQTT Server!!!
   * 
   */
  //WiFi.config(ip, gateway, subnet);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  
  ArduinoOTA.setPassword((const char *)"tico123456");
  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

   /*if (MDNS.begin("esp8266tanque")) {
    Serial.println("MDNS responder started");
  }*/

}



void setupServer(){
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void publicMqttEspAlive(){
  char mystr[40];
  sprintf(mystr,"millis: %u",millis());
  if (client.publish(topicBombaLavaderoEspOK, mystr)) {
    Serial.println("Ok Publish ESP8266ClientOK de bomba lavadero");
  }
  else {
    Serial.println("Fallo en Publish ESP8266ClientOK de bomba lavadero");
  }

  delay(2000);
}

void reconnectMqtt(){
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", usernameMqtt, passwordMqtt)) {
      Serial.println("connected");
      client.subscribe(topicBombaLavadero);
      delay(2000);

      publicMqttEspAlive(); 
    }else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }  

}

void setupMqtt(){
  client.setServer(mqtt_server, serverport);
  client.setCallback(handleMqtt);
  
  reconnectMqtt();  
  
}


void setup() 
{
  // inicializar GPIO 2 como salida 
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  
  conectarAWifiYOtaSetup();
  setupServer();
  setupMqtt();
  
}





void loop() 
{
  ArduinoOTA.handle();
  // Serial.println("OTA rules....");

  server.handleClient();
  //MDNS.update();

  if (!client.connected()) {
    reconnectMqtt();
  }
  
  client.loop();

  /*unsigned long now = millis();
  if ((unsigned long)(now - lastMsg) > 60000) {//1 minuto->60000, 5 minutos->600000
    lastMsg = now;
    publicMqttEspAlive();
  }*/
  
  
  // enciende el LED (HIGH es ALTO y es el nivel de voltaje)
  /*digitalWrite(salida, HIGH);
  delay(1000); // espera 1000 milisegundos = un segundo 
  digitalWrite(salida, LOW); 
  // apaga el LED (LOW es BAJO y es el nivel de voltaje)
  delay(3000); // espera 1000 milisegundos = un segundo
  */
}
  

#include <WiFi.h>
#include "time.h"
#include <Update.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Arduino_JSON.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

// LED
const int led = 2;

// atuadores
const int lamp = 18;          // roxo
const int fan = 5;            // amarelo
const int exaust = 16;        // laranja

/* Device manufacturer */
const char *serial = "GH00000001";
 
/* Constantes - conexão wi-fi e webserver */
const char* host = "greenhouse";
const char* ssid = "VIVOFIBRA-EED0"; /* coloque aqui o nome da rede wi-fi que o ESP32 deve se conectar */
const char* password = "98391748C5"; /* coloque aqui a senha da rede wi-fi que o ESP32 deve se conectar */

// MQTT Broker
const char *mqtt_broker = "automotion.app";
const char *topic = "commands/";
const char *status_topic = "status";
const char *mqtt_username = "erickcrus";
const char *mqtt_password = "12345678";
const int mqtt_port = 8883;


const char echo_org_ssl_ca_cert[] PROGMEM = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n" \
"AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n" \
"ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n" \
"wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n" \
"LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n" \
"4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n" \
"bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n" \
"sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n" \
"Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n" \
"FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n" \
"SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n" \
"PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n" \
"TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n" \
"SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n" \
"c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n" \
"+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n" \
"ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n" \
"b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n" \
"U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n" \
"MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n" \
"5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n" \
"9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n" \
"WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n" \
"he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n" \
"Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

QueueHandle_t QueueHandle;
const int QueueElementSize = 10;
typedef struct {
  char line[128];
  uint8_t line_length;
} message_t;
 
/* Variáveis globais */
int contador_ms = 0;

/* timer configuration */
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3*3600;
const int   daylightOffset_sec = 0;
 
/* Webserver para se comunicar via browser com ESP32  */
WebServer server(80);
 
/* Códigos da página que será aberta no browser 
   (quando comunicar via browser com o ESP32) 
   Esta página exigirá um login e senha, de modo que somente 
   quem tenha estas informações consiga atualizar o firmware
   do ESP32 de forma OTA */
const char* loginIndex = 
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 - identifique-se</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Login:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Senha:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Identificar'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Login ou senha inválidos')"
    "}"
    "}"
"</script>";
  
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>Progresso: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('Progresso: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('Sucesso!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

void setFan(bool status) {
  if(status) {
    digitalWrite(fan, LOW);
  }
  else {
    digitalWrite(fan, HIGH);
  }
}

void setLamp(bool status) {
  if(status) {
    digitalWrite(lamp, LOW);
  }
  else {
    digitalWrite(lamp, HIGH);
  }
}

void setExaust(bool status) {
  if(status) {
    digitalWrite(exaust, LOW);
  }
  else {
    digitalWrite(exaust, HIGH);
  }
}

void proccessQueue( void * pvParameters ){
  message_t message;
  while(true){
    if (QueueHandle != NULL) {
      int ret = xQueueReceive(QueueHandle, &message, 10000);
      if (ret == pdPASS) {
        String msg = message.line;
        JSONVar obj = JSON.parse(msg);

        if(JSON.typeof(obj) == "undefined"){
          logAction(msg);
          return;
        }

        if (obj.hasOwnProperty("lamp")) {
          setLamp((bool) obj["lamp"]);
        }

        if (obj.hasOwnProperty("exaust")) {
          setExaust((bool) obj["exaust"]);
        }

        if (obj.hasOwnProperty("fan")) {
          setFan((bool) obj["fan"]);
        }

      } else if (ret == pdFALSE) {
        delay(10000);
      }

      JSONVar outputStatus;
      outputStatus["serial"] = serial;
      outputStatus["fanStatus"] = !digitalRead(fan);
      outputStatus["lampStatus"] = !digitalRead(lamp);
      outputStatus["exaustStatus"] = !digitalRead(exaust);
      outputStatus["internalTemp"] = round((temprature_sens_read() - 32) / 1.8);

      String output_message = JSON.stringify(outputStatus);
      int output_len = output_message.length();
      byte publish_byte[output_len];
      output_message.getBytes(publish_byte, output_len);
      client.publish(status_topic, (char*) publish_byte);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (QueueHandle != NULL && uxQueueSpacesAvailable(QueueHandle) > 0) {
    char str[length + 1];
    memcpy(str, payload, length);
    str[length] = 0;

    int ret = xQueueSend(QueueHandle, (void *)&str, 0);
    if (ret == errQUEUE_FULL) {
      Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
    }
  }
}
 
void setup(void) {
  // Atuadores
  pinMode(led, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(lamp, OUTPUT);
  pinMode(exaust, OUTPUT);

  Serial.begin(115200);

  /* Conecta-se a rede wi-fi */
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  wifiClient.setCACert(echo_org_ssl_ca_cert);
  Serial.println("");
  Serial.print("Conectado a rede wi-fi ");
  Serial.println(ssid);
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());

  /* Usa MDNS para resolver o DNS */
  if (!MDNS.begin(host)) 
  { 
    //http://greenhouse.local
    Serial.println("Erro ao configurar mDNS. O ESP32 vai reiniciar em 1s...");
    delay(1000);
    ESP.restart();        
  }
  
  Serial.println("mDNS configurado e inicializado;");
  
  /* Configfura as páginas de login e upload de firmware OTA */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
    
  server.on("/serverIndex", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
  });
  
  /* Define tratamentos do update de firmware OTA */
  server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
  }, []() {
      HTTPUpload& upload = server.upload();
        
      if (upload.status == UPLOAD_FILE_START) 
      {
          /* Inicio do upload de firmware OTA */
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) 
              Update.printError(Serial);
      } 
      else if (upload.status == UPLOAD_FILE_WRITE) 
      {
          /* Escrevendo firmware enviado na flash do ESP32 */
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) 
              Update.printError(Serial);      
      } 
      else if (upload.status == UPLOAD_FILE_END) 
      {
          /* Final de upload */
          if (Update.end(true))             
              Serial.printf("Sucesso no update de firmware: %u\nReiniciando ESP32...\n", upload.totalSize);
          else
              Update.printError(Serial);
      }   
  });
  server.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while(!client.connected()) {
    String client_id = serial;
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)){
      Serial.println("Connected into automotion server");
    }
    else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  char s [40];
  strcpy (s, topic);
  strcat (s, serial);
  client.subscribe(s);

  QueueHandle = xQueueCreate(QueueElementSize, sizeof(message_t));
  while (QueueHandle == NULL) {
    Serial.println("Queue could not be created. Halt.");
    delay(1000);  // Halt at this point as is not possible to continue
  }

  delay(10000);
  xTaskCreatePinnedToCore(
    proccessQueue,
    "Status Monitor",
    10000,
    NULL,
    1,
    NULL,
    0
  );
}
 
void loop() {
  server.handleClient();
  client.loop();
}

// https://cplusplus.com/reference/ctime/strftime/
void logAction(String message){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println(message);
    return;
  }

  Serial.print("LOG [");
  Serial.print(&timeinfo, "%F %T");
  Serial.print("]: ");
  Serial.println(message);
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

int getHour(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    logAction("Failed to obtain time");
    return 0;
  }

  char _time[ 5 ];
  strftime( _time, 5, "%H", &timeinfo );
  return String(_time).toInt();
}
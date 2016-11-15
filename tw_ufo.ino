// todo : bring back NTP, or make API require timestamps be passed in
// todo: show last 10 build results in ufo dashboard
// todo: integrate dotstar led library
// todo: fun mattermost integrations like /karma, or /party

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <FS.h>
#include <ArduinoJson.h>
#define DEBUG_ESP_HTTP_SERVER true
//const char *ssid = "Teamworks";
//const char *password = "125121";
const char *ssid = "blazeone_gb";
const char *password = "52105210";

#define DBG_OUTPUT_PORT Serial
HTTPClient http;

 char* statusList[] = {
  "CONNECTING", 
  "BUILDING", 
  "DEPLOYING",
  "TESTING",
  "STABLE",
  "UNSTABLE",
  "FAILED",
  "ALARM"  
};

const char* environmentList[] = {
  "SPRINT", 
  "RELEASE",
  "PRODUCTION",
  "STAGING",
  "LATEST"
};



int address = 0;
int statusData[5] = {0,0,0,0,0};


ESP8266WebServer server ( 80 );
File fsUploadFile;

const int led = 13;

//format bytes
String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}


String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}



bool handleFileRead(String path){
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleNotFound() {
  if(!handleFileRead(server.uri())){
    server.send ( 404, "text/plain", "File Not Found" );
  }
}


String handleVersion(String serverName, String url){ 
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  String json;
  
  root[serverName] = requestVersion(url);
  
  root.printTo(json);
  Serial.println(json);
  return json;
}

void handleLatest(){
  String json = handleVersion("latest", "http://latest.teamworksapp.com/version.txt");
  server.send ( 200, "application/json", json); 
}

void handleStaging(){
 String json = handleVersion("staging", "https://staging.teamworksapp.com/version.txt");
  server.send ( 200, "application/json", json); 
}

void handleProduction(){
  String json = handleVersion("production", "https://www.teamworksapp.com/version.txt");      
  server.send ( 200, "application/json", json); 
}



String requestVersion( String serverURL ){
   String versionText;
   
   http.begin(serverURL);  
   int httpCode = http.GET();
   if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
        versionText = http.getString();
        http.end();
        return(versionText);
      }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    return( http.errorToString(httpCode).c_str() );
  }
    
}


void handleInfo(){
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  String json;
    
  root["name"] = "TW UFO";
  root["description"] = "Teamworks build status monitor";
  
  JsonArray& statusData = root.createNestedArray("statuses");
  statusData.add("CONNECTING"); 
  statusData.add("BUILDING"); 
  statusData.add("DEPLOYING");
  statusData.add("TESTING");
  statusData.add("STABLE");
  statusData.add("UNSTABLE");
  statusData.add("FAILED");
  statusData.add("ALARM");

  JsonArray& environmentData = root.createNestedArray("environments");
  // Environments
  environmentData.add("PRODUCTION"); 
  environmentData.add("STAGING"); 
  environmentData.add("LATEST"); 

  // Pipelines
  environmentData.add("SPRINT"); 
  environmentData.add("RELEASE"); 
  
  root.printTo(json);

  server.send ( 200, "application/json", json);
}



void handleStatus(){ 

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  String json;
  
  if ( server.method() == HTTP_OPTIONS){ 
    const String header = "HTTP/1.1 200 OK\r\nContent-type: application/json\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nAccess-Control-Max-Age: 86400\r\nAccess-Control-Allow-Headers: Api-Key,Content-type\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    Serial.println(header);
    
    server.sendContent(header);
    Serial.println("Sent CORS header");
     root["success"] = true;
  }
  
 if ( server.method() == HTTP_GET){ 
      root["sprint"] = statusList[statusData[0]];  
      root["release"] =  statusList[statusData[1]];  
      root["production"] =  statusList[statusData[2]];  
      root["staging"] =  statusList[statusData[3]];  
      root["latest"] =  statusList[statusData[4]];
        
  }

  if ( server.method() == HTTP_POST){ 
   
    // parse the json body
    char *jsonData = new char[500];
    server.arg(0).toCharArray(jsonData, 500);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonData);

    if (!root.success()){
        return server.send(500, "text/plain", "BAD REQUEST BODY");
    }

    String environment = root["environment"];
    String buildStatus = root["status"];

    
    int environmentPointer = 0; 
    int statusPointer = 0; 
        
    Serial.println( environment + " transitioned to " + buildStatus);
    
    int environmentLength = sizeof(environment);
    
    for (int i=0; i<environmentLength; i++) {
      if (environment == environmentList[i]) {
         environmentPointer = i;
        break;
      }
    }
    
    int statusLength = sizeof(statusList)/sizeof(statusList[0]);
    for (int i=0; i<statusLength; i++) {
      if (buildStatus == statusList[i]) {
         statusPointer = i;
        break;
      }
    }
     
    statusData[environmentPointer] = statusPointer;
    root["success"] = true;
    updateEEPROM();
  }
  
  root.printTo(json);
  server.send ( 200, "application/json", json );
  
}

void updateEEPROM(){
  
   for ( int i = 0; i < sizeof(statusData); i++){
    EEPROM.write(i, statusData[i]);
   }

    EEPROM.commit();
}
  
void setup ( void ) {

  pinMode ( led, OUTPUT );
  digitalWrite ( led, 0 );
  Serial.begin ( 9600 );
  EEPROM.begin(512);

  for ( int i = 0; i < sizeof(statusData); i++){
    statusData[i] = EEPROM.read(i);
  }
  
  // mount the filesystem
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
  }

  // start the wifi'z
  
  WiFi.begin ( ssid, password );
  Serial.println ( "" );
  
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

 
  server.on ( "/info", handleInfo );
  server.on ( "/status", handleStatus );
  server.on ( "/version/latest", handleLatest );
  server.on ( "/version/staging", handleStaging );
  server.on ( "/version/production", handleProduction);
  
  server.on ( "/test.svg", drawGraph );
  
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println ( "HTTP server started" );
}


void loop ( void ) {
  server.handleClient();
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}


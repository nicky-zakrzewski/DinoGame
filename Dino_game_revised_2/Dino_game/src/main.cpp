#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";
const char* PARAM_INPUT_5 = "console";


//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;
String console;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";
const char* consolePath = "/console.txt";

IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// Set LED GPIO
const int ledPin = 2;
// Stores LED state

String ledState;

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());


  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}

// Replaces placeholder with LED state value
String processor(const String& var) {
  if(var == "STATE") {
    if(digitalRead(ledPin)) {
      ledState = "ON";
    }
    else {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}


/*Arduino Chrome Dino (T-Rex) Game on a 16-pin 16x2 LCD Display
  Code used in the Dino Game project from YouTube Short/Instagram Reel video

  Author credits (YT Channel): CREA ELECTRONICA
  Original video: https://www.youtube.com/watch?v=yki4MBmUFI4&t=2s

  Code revised by Kris Houben
  Code revised by Phuriphat Maneethap & Nicky Zakrzewski
 */

#include <LiquidCrystal.h>
const int rs = 18, en = 13, d4 = 17, d5 = 16, d6 = 27, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte DINO_FEET_RIGHT_PART_1[8] = {B00000, B00000, B00010, B00010, B00011, B00011, B00001, B00001};
byte DINO_FEET_RIGHT_PART_2[8] = {B00111, B00111, B00111, B00100, B11100, B11100, B11000, B00000};
byte DINO_FEET_LEFT_PART_1[8] = {B00000, B00000, B00010, B00010, B00011, B00011, B00001, B00000};
byte DINO_FEET_LEFT_PART_2[8] = {B00111, B00111, B00111, B00100, B11100, B11100, B11000, B01000};
byte CACTUS_SMALL[8] = {B00000, B00100, B00100, B10100, B10100, B11100, B00100, B00100};
byte CACTUS_BIG[8] = {B00100, B00101, B00101, B10101, B11111, B00100, B00100, B00100};
byte BIRD_PART1[8] = {B00001, B00001, B00001, B00001, B01001, B11111, B00000, B00000};
byte BIRD_PART2[8] = {B00000, B10000, B11000, B11100, B11110, B11111, B00000, B00000};

// variable declarations
int button = 25;              // Pushbutton pin - for making the Dinosaur jump
int buzzer = 4;               // Passive Buzzer pin - for sound effects

int button2 = 35;

int gamelevel = 0;
int gamescore = 0;

int column_dino_part1 = 1;    // column position part1 of dino
int column_dino_part2 = 2;    // column position part2 of dino 
int row_dino = 1;             // row position for dino
int dinofeetposition = 1;     // flag for dino feet position : 1 = right , 2 = left
int dinostate = 0;            // flag for dino state : 0 = running , 1 = flying 

int objecttype = 0;           // 0 = bird , 1 = small cactus , 2 = big cactus
int row_object = 0;           // row position of object (0 = top row , 1 = bottom row)
int column_object = 13;       // column position of object
int objectspeeddelay = 300;   // initial value for object speed delay
int objectaccelaration = 5;   // object acceleration each round in milliseconds    
int newobjectposition = 0;    // flag for detection new object position  : 1 = set new position , 0 = keep current position 
int randomrow = 0;            // randomrow for bird objecttype
// timers
// timers are used with millis() function
// timer period in milliseconds
unsigned long dinofeettimer = 0;      // dino feet delay
int dinofeetperiod = 200;
unsigned long objectspeedtimer = 0;   // object speed delay
int objectspeedperiod = objectspeeddelay;
unsigned long gamescoretimer = 0;     // gamescore delay
int gamescoreperiod = 200;

void gameover() {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(0, 1);
    lcd.print("Score : L"+String(gamelevel)+" - "+String(gamescore));
    int note[] = {200, 150};
    for (int i = 0; i < 2; i++) {
      tone(buzzer, note[i], 250);
      delay(200);
    }
    delay(3000);
    lcd.clear();
    column_object = 13;
    objectspeedperiod = objectspeeddelay;
    gamescore = 0;
    gamelevel = 0;
}
int buttonstate(){
  int b = 0;
  int x = analogRead(35);
  if (200 < x < 400){
    b = 1;
  }
  else{
    b = 0;
  }
  return b;
}


void setup() {
 
  //for (int i = 2; i <= 17; i++)  pinMode(i, OUTPUT);
  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  
  //lcd.begin();
  lcd.begin(16,2);
  lcd.clear();
  lcd.createChar(0, DINO_FEET_RIGHT_PART_1);
  lcd.createChar(1, DINO_FEET_RIGHT_PART_2);
  lcd.createChar(2, DINO_FEET_LEFT_PART_1);
  lcd.createChar(3, DINO_FEET_LEFT_PART_2);
  lcd.createChar(4, CACTUS_SMALL);
  lcd.createChar(5, CACTUS_BIG);
  lcd.createChar(6, BIRD_PART1);
  lcd.createChar(7, BIRD_PART2);

  //WIFI SETUP


  // Serial port for debugging purposes
  Serial.begin(115200);

  initSPIFFS();

  // Set GPIO as an OUTPUT
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile (SPIFFS, gatewayPath);
  console = readFile (SPIFFS, consolePath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);
  Serial.println(console);

  if(initWiFi()) {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });
    server.serveStatic("/", SPIFFS, "/");
    
    // Route to set GPIO state to HIGH
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
      digitalWrite(ledPin, HIGH);
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });

    // Route to set GPIO state to LOW
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
      digitalWrite(ledPin, LOW);
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });
    server.begin();
  }
  else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(SPIFFS, gatewayPath, gateway.c_str());
          }
          //HTTP POST console value
          if (p->name() == PARAM_INPUT_5) {
            console = p->value().c_str();
            Serial.print("Console set to: ");
            Serial.println(console);
            // Write file to save value
            writeFile(SPIFFS, consolePath, console.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(3000);
      ESP.restart();
    });
    server.begin();
  }

}

void loop() {
  digitalWrite(ledPin, HIGH);
if (digitalRead(button2) == true) {
  digitalWrite(ledPin, LOW);
}


  if (millis() > dinofeettimer + dinofeetperiod) { //delay for changing dino feet position
    dinofeettimer = millis();
    if (dinofeetposition == 1) {
      dinofeetposition = 2;
    }
    else if (dinofeetposition == 2) {
      dinofeetposition = 1;
    }
  }

  if (millis() > objectspeedtimer + objectspeedperiod) { // delay for object speed
    objectspeedtimer = millis();
    column_object = column_object - 1;
    if (column_object < 0) {
      column_object = 13;
      objectspeedperiod = objectspeedperiod - objectaccelaration;  // acceleration 
      objecttype = random(0, 3); // choose number between 0 and 2
      randomrow = random(0, 2);  // choos number between 0 and 1
    }
    
    if (column_object < 13) {
      lcd.setCursor(column_object+1, 1);    //clean down position
      lcd.print(" ");
      lcd.setCursor(column_object+1, 0);    //clean up position
      lcd.print(" ");
    }
    
    lcd.setCursor(0, 1);   //clean down position
    lcd.print(" ");
    lcd.setCursor(0, 0);  // clean up position
    lcd.print(" ");
    newobjectposition = 1;
  }

  if (dinostate == 0) { 
    if (dinofeetposition == 1) {  // dino feet right
      lcd.setCursor(column_dino_part1, row_dino);
      lcd.write(byte(0));
      lcd.setCursor(column_dino_part2, row_dino);
      lcd.write(byte(1));
    }
    if (dinofeetposition == 2) {  // dino feet left
      lcd.setCursor(column_dino_part1, row_dino);
      lcd.write(byte(2));
      lcd.setCursor(column_dino_part2, row_dino);
      lcd.write(byte(3));
    }
  }

  if (newobjectposition == 1) {
    if (objecttype == 1) {  // small cactus
      row_object = 1;
      lcd.setCursor(column_object, row_object);
      lcd.write(byte(4));

    }
    else if (objecttype == 2) { // big cactus
      row_object = 1;
      lcd.setCursor(column_object, row_object);
      lcd.write(byte(5));

    }
    else {      // bird 
      row_object = randomrow;
      lcd.setCursor(column_object-1, row_object);
      lcd.write(byte(6));
      lcd.setCursor(column_object, row_object); 
      lcd.write(byte(7));

    }
    newobjectposition = 0;
  }
  
  // colliding conditions for upper row
  if (digitalRead(ledPin) == 0 && (column_object == 1 || column_object == 2 ) && row_object == 0 ) {
    gameover();
  }

  //colliding conditions for bottom row
  if (digitalRead(ledPin) == 1 && (column_object == 1 || column_object == 2 ) && row_object == 1 ) {
    gameover(); 
  }

  int currentdinostate = dinostate;
  if (digitalRead(ledPin) == 0) {
    if (dinostate == 0) {
      lcd.setCursor(0, 1);  // remove dino from bottom row
      lcd.print("    ");
      dinostate = 1;
    }
    lcd.setCursor(column_dino_part1, 0);
    lcd.write(byte(0));
    lcd.setCursor(column_dino_part2, 0);
    lcd.write(byte(1));
    
    if (currentdinostate == 0) { 
      int note[] = {600};
      for (int i = 0; i < 1; i++) {
        tone(buzzer, note[i], 150);
        delay(20);
      }
    }
  }
  else {
    if (currentdinostate == 1) {
      lcd.setCursor(0, 0);   // remove dino from top row
      lcd.print("    ");  
    }
    dinostate = 0;
  }

  //gamescore
  if (millis() > gamescoretimer + gamescoreperiod) { // delay for gamescore
    gamescoretimer = millis();
    gamescore = gamescore + 1;
    if (gamescore == 100) {
      int note[] = {800, 900};
      for (int i = 0; i < 2; i++) {
        tone(buzzer, note[i], 150);
        delay(150);
      }
      gamescore = 0;
      gamelevel = gamelevel + 1;
      if (gamelevel == 100) {
        gamelevel = 0;
      }
    }
    lcd.setCursor(14, 1);
    lcd.print(gamescore);
    lcd.setCursor(14, 0);
    lcd.print(gamelevel);
  }
}

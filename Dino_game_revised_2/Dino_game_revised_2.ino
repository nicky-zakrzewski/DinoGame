

/*Arduino Chrome Dino (T-Rex) Game on a 16-pin 16x2 LCD Display
  Code used in the Dino Game project from YouTube Short/Instagram Reel video

  Author credits (YT Channel): CREA ELECTRONICA
  Original video: https://www.youtube.com/watch?v=yki4MBmUFI4&t=2s

  Code revised by Kris Houben
  Code revised by Phuriphat Maneethap & Nicky Zakrzewski
 */

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Fill out pin declarations according to board pins the LCD is connected to

byte DINO_FEET_RIGHT_PART_1[8] = {B00000, B00000, B00010, B00010, B00011, B00011, B00001, B00001};
byte DINO_FEET_RIGHT_PART_2[8] = {B00111, B00111, B00111, B00100, B11100, B11100, B11000, B00000};
byte DINO_FEET_LEFT_PART_1[8] = {B00000, B00000, B00010, B00010, B00011, B00011, B00001, B00000};
byte DINO_FEET_LEFT_PART_2[8] = {B00111, B00111, B00111, B00100, B11100, B11100, B11000, B01000};
byte CACTUS_SMALL[8] = {B00000, B00100, B00100, B10100, B10100, B11100, B00100, B00100};
byte CACTUS_BIG[8] = {B00100, B00101, B00101, B10101, B11111, B00100, B00100, B00100};
byte BIRD_PART1[8] = {B00001, B00001, B00001, B00001, B01001, B11111, B00000, B00000};
byte BIRD_PART2[8] = {B00000, B10000, B11000, B11100, B11110, B11111, B00000, B00000};

// variable declarations
int button = 14;              // Pushbutton pin - for making the Dinosaur jump
int buzzer = 10;               // Passive Buzzer pin - for sound effects

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

void setup() {
 
  for (int i = 2; i <= 17; i++)  pinMode(i, OUTPUT);
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


}

void loop() {
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
  if (digitalRead(button) == false && (column_object == 1 || column_object == 2 ) && row_object == 0 ) {
    gameover();
  }

  //colliding conditions for bottom row
  if (digitalRead(button) == HIGH && (column_object == 1 || column_object == 2 ) && row_object == 1 ) {
    gameover(); 
  }
/*
  int currentdinostate = dinostate;
  if (digitalRead(button) == true) {
    if (dinostate == 0) {
      lcd.setCursor(0, 1);  // remove dino from bottom row
      lcd.print("    ");
      dinostate = 1;
    }
    */
      int currentdinostate = dinostate;
  if (digitalRead(button) == false) {
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

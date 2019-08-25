// Davide Caminati Demo Microscope 2019
// Demo based on:
// UTFT_Demo by Henning Karlsen
// web: http://www.henningkarlsen.com/electronics

#include <Ucar.h>
#include "Adafruit_GFX_AS.h"
#include "Adafruit_R61581_AS.h"

/*
  #define R61581_BLACK   0x0000
  #define R61581_BLUE    0x001F  
  #define R61581_RED     0xF800  
  #define R61581_GREEN   0x07E0  
  #define R61581_CYAN    0x07FF
  #define R61581_MAGENTA 0xF81F
  #define R61581_YELLOW  0xFFE0  
  #define R61581_WHITE   0xFFFF
 */
 
Adafruit_R61581_AS myGLCD = Adafruit_R61581_AS();       // Invoke custom library

String linea00 = "000000000000000000000000000000000000000000"; 
String linea01 = "000100000100000001000000100000100000001000"; // to move
String linea02 = "000000000000000000000000000000000000000000";
String linea03 = "111111111111111111111111111111111111111111";
String linea04 = "111111111111111111111111111111111111111111";
String linea05 = "111111111111111111111111111111111111111111";
String linea06 = "111111111111111111111111111111111111111111";
String linea07 = "111111111111111111111111111111111111111111";
String linea08 = "111111111111111111111111111111111111111111";
String linea09 = "000000000000000000000000000000000000000000";
String linea10 = "010101000100010000010010101000100010000010"; // will changes during movement
String arr[11] = {linea00,linea01,linea02,linea03,linea04,linea05,linea06,linea07,linea08,linea09,linea10};

int trackW ;
int trackH ;
int trackLength= 23; // depends on microscope

Ucar car_0(R61581_GREEN, 1, 4, 0, "GREEN"); // color , x , y , penality, name
Ucar car_1(R61581_RED, 1, 5, 0, "RED");
Ucar car_2(R61581_BLUE, 1, 6, 0, "BLUE");
Ucar * cars[] = {&car_0, &car_1, &car_2}; // cars array
Ucar enemy_0(R61581_WHITE, trackLength, 0, 0, ""); // random position
Ucar enemy_1(R61581_WHITE, trackLength, 0, 0, ""); // random position
Ucar * enemies[] = {&enemy_0, &enemy_1}; // enemies array

String cordolo = linea01 + linea01;
int curb = 0;

int actual_speed = 0;
long oldTimerStreet;
long oldTimerCar;
long oldTimerEnemy;

int timerStreet  = 197;
int timerCar = 1019;
int timerEnemy = 139;
bool arrived = false;

void setup(){
  // Initialize serial
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Initialize some pins just for fun
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(10);
  digitalWrite(7, HIGH);
  // Setup the LCD
  myGLCD.begin(R61581B);
  myGLCD.setRotation(3);
  // Number of line for Track height and Width
  trackW = sizeof(arr) / sizeof(arr[0]);
  trackH = arr[0].length();
  // Draw labirint  
  drawTrack();
  // place players
  startPlayers();
  // place enemies
  startEnemies();
  // initialize the timers
  oldTimerCar = millis();
  oldTimerStreet = millis();
  oldTimerEnemy = millis();
}

void loop(){
  if (not arrived){
    // street move
    if ((oldTimerStreet + timerStreet - actual_speed) < millis()){
      moveStreet();
      actual_speed += 2;
      if (actual_speed > 100) actual_speed = 100;
      oldTimerStreet = millis();
    }
    // cars move
    if ((oldTimerCar + timerCar) < millis()){
      for (int c =0 ; c < 3; c++){
        moveCar((*cars[c]),cars[c]->x + 1 - cars[c]->penality,cars[c]->y);
      }
      oldTimerCar = millis();
    }
    // enemy move
    if ((oldTimerEnemy + timerEnemy) < millis()){
      for (int e = 0; e < 2; e++){
        moveEnemy((*enemies[e]),enemies[e]->x - 1,enemies[e]->y);
        checkCollision(*enemies[e]);
      }
      oldTimerEnemy = millis();
    }
    // check arrived (Winner)
    for (int c = 0; c < 3; c++){
      if(cars[c]->x >= trackLength){
        arrived = true;
      }
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar == '0')  cars[0]->penality += 1; // give penality 
    if (inChar == '1')  cars[1]->penality += 1; // give penality 
    if (inChar == '2')  cars[2]->penality += 1; // give penality 
    //car 0
    if (inChar == 'q') moveCar((*cars[0]),cars[0]->x,cars[0]->y -1);  // up
    if (inChar == 'a') moveCar((*cars[0]),cars[0]->x,cars[0]->y +1);  // down
    //car 1
    if (inChar == 'w') moveCar((*cars[1]),cars[1]->x,cars[1]->y -1);
    if (inChar == 's') moveCar((*cars[1]),cars[1]->x,cars[1]->y +1);
    //car 2
    if (inChar == 'e') moveCar((*cars[2]),cars[2]->x,cars[2]->y -1);
    if (inChar == 'd') moveCar((*cars[2]),cars[2]->x,cars[2]->y +1);
  }
}

void checkCollision(Ucar &enemy){
    for (int c = 0; c < 3; c++){
      if ((enemy.x == (*cars[c]).x) && (enemy.y == (*cars[c]).y)) {
        // collision
        cars[c]->penality += 1;
        enemy.x = trackLength + random(2,10);
        enemy.y = random(3,7);
    }
  }
}

void drawTrack(){
  myGLCD.fillScreen(R61581_WHITE); // background
  for (int x = 0; x < trackH; x++){
    for (int y = 0; y < trackW; y++){
      if (arr[y].charAt(x) == '1') {
        myGLCD.drawLine(x, y, x, y,R61581_BLACK);
      }
    }
  }
}

void startPlayers(){
  for (int c = 0; c < 3; c++){
    moveCar(*cars[c],cars[c]->x,cars[c]->y);
  }
}

void startEnemies(){
  for (int e = 0; e < 2; e++){
    moveCar((*enemies[e]),enemies[e]->x += random(1,10),enemies[e]->y += random(3,7));
  }
}

bool canMove(int x,int y){
  return ((y > 2) && (y < 9) && (x > -1) && (x < trackLength+1));
}

void moveCar(Ucar &car,int x, int y){
  int old_color = 0x0000;
  int new_color = 0x0000;
  int old_x = car.x;
  int old_y = car.y;
  // search for other pixel in same (old) place and get color sum
  for (int c = 0; c < 3; c++){
    if((old_x == cars[c]->x) && (old_y == cars[c]->y)){
      old_color += cars[c]->color;
    }
  }
  // remove my color value to the sum in old pixel
  old_color -= car.color;
  myGLCD.drawPixel(old_x, old_y, old_color); // draw old pixel
  car.x = x;
  car.y = y;
  // search for other pixel in same (new) place and get color sum
  for (int c = 0; c < 3; c++){
    if((x == cars[c]->x) && (y == cars[c]->y)){
      new_color += cars[c]->color;
    }
  }
  // add my color value to the sum in new pixel
  myGLCD.drawPixel(x, y, new_color); // draw new pixel
  car.penality = 0;
}

void moveEnemy(Ucar &car,int x, int y){
  // delete car in old position
  myGLCD.drawPixel(car.x, car.y, R61581_BLACK);
  // move car in new position
  car.x = x;
  car.y = y;
  // draw car in new position
  myGLCD.drawPixel(car.x, car.y, car.color);
  if (car.x < 0) {
    car.y = random(3, 7);
    car.x = trackLength + random(1,10);
  }
}

void moveStreet(){
  for (int x = 0; x < trackH-1 ; x++){
    if (cordolo.charAt(x+curb) == '1') {
      myGLCD.drawLine(x, 1, x, 1,R61581_BLACK);
      myGLCD.drawLine(x, 10, x, 10,R61581_BLACK);
    }
    else{
      myGLCD.drawLine(x, 1, x, 1,R61581_WHITE);
      myGLCD.drawLine(x, 10, x, 10,R61581_WHITE);
    }
  }
  curb = (curb < trackH-1) ? curb += 1:0;
}

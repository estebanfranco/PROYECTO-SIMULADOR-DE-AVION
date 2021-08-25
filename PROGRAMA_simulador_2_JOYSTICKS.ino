/*
   CSCI 435A Embedded Systems
   Authors: Angad Virk, Zale Young
   
   Video Game Controller
   Report Website: https://go.middlebury.edu/sbox
*/

#include <Arduino.h>
#include <XInput.h>

// Define Pin number constants
#define UP_BUTTON 4
#define DOWN_BUTTON 5
#define LEFT_BUTTON 6
#define RIGHT_BUTTON 7

#define Y_BUTTON 8
#define A_BUTTON 9
#define X_BUTTON 10
#define B_BUTTON 11

#define INVERT_LEFT false  // set to true to use inverted left joy Y
#define LEFT_JOY_X A0
#define LEFT_JOY_Y A1
#define LEFT_JOY_SEL 2

#define INVERT_RIGHT true  // set to true to use inverted right joy Y
#define RIGHT_JOY_X A2
#define RIGHT_JOY_Y A3
#define RIGHT_JOY_SEL 3

#define ANALOG_MAX 1023 // Analog Input returns values from 0 to 1023 for joystick moves

#define JOY_MOVE_THRESH 10 // Threshold to account for noise in potentiometer voltage when joystick is at rest

#define DEBUG true

void setup() {

  //TODO: CHANGE TO INPUT_PULLUP AND CONNECT TO GROUND AND USE ACTIVE
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  
  pinMode(Y_BUTTON, INPUT_PULLUP);
  pinMode(A_BUTTON, INPUT_PULLUP);
  pinMode(X_BUTTON, INPUT_PULLUP);
  pinMode(B_BUTTON, INPUT_PULLUP);

  pinMode(LEFT_JOY_X, INPUT);
  pinMode(LEFT_JOY_Y, INPUT);
  pinMode(LEFT_JOY_SEL, INPUT_PULLUP);

  pinMode(RIGHT_JOY_X, INPUT);
  pinMode(RIGHT_JOY_Y, INPUT);
  pinMode(RIGHT_JOY_SEL, INPUT_PULLUP);


  Serial.begin(115200);
  
  XInput.setJoystickRange(0, 1023);
  XInput.setAutoSend(false);

  XInput.begin();
}

// Variables to store the state of the button inputs
boolean up, down, left, right;
boolean button_y, button_a, button_x, button_b;

int leftx, lefty;
int rightx, righty;
boolean leftz, rightz;

int absolute(int x){
  return x < 0 ? -x : x;
}

// Function to read the state of all inputs. 
// Buttons are active low. Joysticks are analog inputs
void readState(){
  // Read state of Directional Buttons
  left = digitalRead(LEFT_BUTTON) == LOW;
  right = digitalRead(RIGHT_BUTTON) == LOW;
  up = digitalRead(UP_BUTTON) == LOW;
  down = digitalRead(DOWN_BUTTON) == LOW;

  // Read state of Action Buttons
  button_y = digitalRead(Y_BUTTON) == LOW;
  button_a = digitalRead(A_BUTTON) == LOW;
  button_x = digitalRead(X_BUTTON) == LOW;
  button_b = digitalRead(B_BUTTON) == LOW;

  // Read State of Joystick and Joystick Buttons
  leftx = analogRead(LEFT_JOY_X);
  lefty = analogRead(LEFT_JOY_Y);
  leftz = digitalRead(LEFT_JOY_SEL) == LOW;

  rightx = analogRead(RIGHT_JOY_X);
  righty = analogRead(RIGHT_JOY_Y);
  rightz = digitalRead(RIGHT_JOY_SEL) == LOW;
}

// Set the state of the directional pad based on button inputs
void setDPads(){
  XInput.setDpad(up, down, left, right);

  #if DEBUG 
    if(up){
      Serial.println("up");
    }
    if(right){
      Serial.println("right");
    }
    if(down){
      Serial.println("down");
    }
    if(left){
      Serial.println("left");
    }
  #endif
}

// Sets the state of the button based on condition (state of the input)
void conditionalPress(boolean condition, int button, char* message){
  XInput.setButton(button, condition);

  #if DEBUG 
    if(condition){
      Serial.println(message);
    }
  #endif
}

// Set the state of the Joystick buttons and the A,B,X,Y buttons
void setButtons(){
  conditionalPress(leftz, TRIGGER_LEFT, "LeftJoyButton");
  conditionalPress(rightz, TRIGGER_RIGHT, "RightJoyButton");

  conditionalPress(button_y, BUTTON_Y, "YButton");
  conditionalPress(button_a, BUTTON_A, "AButton");
  conditionalPress(button_x, BUTTON_X, "XButton");
  conditionalPress(button_b, BUTTON_B, "BButton");

}

// Set the state of the joysticks. 
void setJoy(){
  int joy_center = ANALOG_MAX/2;

  // Account for imperfect center voltages by ensurint that the joystick 
  // is moved beyond a certain amount/threshold
  int leftXValue =  absolute(leftx - joy_center) > JOY_MOVE_THRESH ?
                    leftx :
                    joy_center;
                    
  int leftYValue =  absolute(lefty - joy_center) > JOY_MOVE_THRESH ?
                    lefty :
                    joy_center;
                    
  int rightXValue =  absolute(rightx - joy_center) > JOY_MOVE_THRESH ?
                    rightx :
                    joy_center;
                    
  int rightYValue =  absolute(righty - joy_center) > JOY_MOVE_THRESH ?
                    righty :
                    joy_center;                              

  // Invert Joystick movement if the variables are set.
  // Our right joystick is upside down so we have set INVERT_RIGHT to true above. 
  if(INVERT_LEFT){
    leftXValue = ANALOG_MAX - leftXValue;
    leftYValue = ANALOG_MAX - leftYValue;
  }

  if(INVERT_RIGHT){
    rightXValue = ANALOG_MAX - rightXValue;
    rightYValue = ANALOG_MAX - rightYValue;
  }

  // Set Left Joystick state 
  XInput.setJoystick(JOY_LEFT, leftXValue, leftYValue);
  XInput.setJoystick(JOY_RIGHT, rightXValue, rightYValue);
  
  #if DEBUG
    Serial.println(leftx);
    Serial.println(lefty);
    Serial.println(rightx);
    Serial.println(righty);
  #endif
  
}

void loop() {
  // READ IN THE STATE AND SET GLOBAL VARIABLES
  readState();

  // Set XInput States
  setDPads();
  setButtons();
  setJoy();

  XInput.send();

  //simple debounce
  delay(100);

} 

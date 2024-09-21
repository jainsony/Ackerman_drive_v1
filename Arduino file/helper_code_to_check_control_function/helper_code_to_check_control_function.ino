// I am Reciever
// This micro-controller MAC : ec:fa:bc:c1:e6:ea

#include <ESP8266WiFi.h>
#include <espnow.h>


uint8_t debug_mode = 1;

#define L_PWM_motor_1 5   // D1 Left PWM for Motor 1 
#define R_PWM_motor_1 4   // D2 Right PWM for Motor 1 

#define L_PWM_motor_2 0   // D3 Left PWM for Motor 2
#define R_PWM_motor_2 2   // D4 Right PWM for Motor 2 

#define datalen 4
#define threshold 5

int i = 0;

int a_speed = 0;
int b_speed = 0;

int Speed = 0;

String dataString = ""; // Variable to hold input data
int dataArray[datalen]; // Array to hold parsed integers
int dataIndex = 0; // Index for dataArray

String success;

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(L_PWM_motor_1, OUTPUT);
  pinMode(R_PWM_motor_1, OUTPUT);
  pinMode(L_PWM_motor_2, OUTPUT);
  pinMode(R_PWM_motor_2, OUTPUT);
 
}

void get_serial_reading()
{
  if (Serial.available()) { // check if there is incoming data
    char incomingChar = Serial.read(); // read incoming character
    // Serial.print(incomingChar);
    if (incomingChar == '<') { // start of data
      dataString = ""; // clear input data
      dataIndex = 0; // reset index for dataArray
    } else if (incomingChar == '>') { // end of data
      parseData(); // call function to parse input data
      // printData();  // call function to print parsed data
      // control();
    } else { // data character
      dataString += incomingChar; // add to input data
    }
}
}

void parseData() {
  int commaIndex = 0; // index for locating commas in input string
  while (dataIndex < datalen) { // parse up to 4 integers
    commaIndex = dataString.indexOf(","); // find next comma in input string
    if (commaIndex == -1) { 
      commaIndex = dataString.length();
    }
    dataArray[dataIndex] = dataString.substring(0, commaIndex).toInt(); // convert substring to integer and store in array
    dataString = dataString.substring(commaIndex + 1); // remove parsed substring and comma from input string
    dataIndex++; // move to next index in array
  }
}

void printData() {
  Serial.print("Parsed data: ");
  for (i = 0; i < datalen; i++) {
    Serial.print(dataArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}
 
void loop() {
  get_serial_reading();
  printData();
  Speed = dataArray[0];
  control();
}

// Control the motors using two BTS7960 drivers
void control() {
  if (Speed > 0) {
    // Move forward
    a_speed = Speed;
    analogWrite(L_PWM_motor_1, 0);
    analogWrite(R_PWM_motor_1, a_speed);
    analogWrite(L_PWM_motor_2, 0);
    analogWrite(R_PWM_motor_2, a_speed);
  } 
  else if (Speed < 0) {
    // Move backward
    a_speed = -1 * Speed;
    analogWrite(L_PWM_motor_1, a_speed);
    analogWrite(R_PWM_motor_1, 0);
    analogWrite(L_PWM_motor_2, a_speed);
    analogWrite(R_PWM_motor_2, 0);
  } 
  // else if (Direction > threshold) {
  //   // Turn right
  //   b_speed = Direction;
  //   analogWrite(L_PWM_motor_1, b_speed);
  //   analogWrite(R_PWM_motor_1, 0);  // Stop right motor 1
  //   analogWrite(L_PWM_motor_2, b_speed);
  //   analogWrite(R_PWM_motor_2, 0);  // Stop right motor 2
  // } 
  // else if (Direction < -threshold) {
  //   // Turn left
  //   b_speed = -1 * Direction;
  //   analogWrite(L_PWM_motor_1, 0);  // Stop left motor 1
  //   analogWrite(R_PWM_motor_1, b_speed);
  //   analogWrite(L_PWM_motor_2, 0);  // Stop left motor 2
  //   analogWrite(R_PWM_motor_2, b_speed);
  // } 
  else {
    // Stop
    analogWrite(L_PWM_motor_1, 0);
    analogWrite(R_PWM_motor_1, 0);
    analogWrite(L_PWM_motor_2, 0);
    analogWrite(R_PWM_motor_2, 0);
    Speed = 0;
  }
}

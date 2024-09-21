// I am Reciever
// This micro-controller MAC : {0xA4, 0xCF, 0x12, 0xD9, 0x24, 0xDE}

#include <ESP8266WiFi.h>
#include <espnow.h>

// Other controller MAC
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0xD5, 0xD4, 0xC1};

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
int Direction = 0;

// Define variables to store incoming readings
float incomingSpeed;
float incomingDir;

String dataString = ""; // Variable to hold input data
int dataArray[datalen]; // Array to hold parsed integers
int dataIndex = 0; // Index for dataArray

const long interval = 100; 
unsigned long previousMillis = 0;    // Will store last time DHT was updated 

String success;

typedef struct struct_message {
    float speed;
    float dir;
} struct_message;

struct_message Data_structure;
struct_message incomingReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (debug_mode == 1)
    Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    if (debug_mode == 1)
      Serial.println("Delivery success");
  } else {
    if (debug_mode == 1)
      Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  if (debug_mode == 1) {
    Serial.print("Bytes received: ");
    Serial.println(len);
  }

  incomingSpeed = incomingReadings.speed;
  incomingDir = incomingReadings.dir;
}

// Get incoming data for speed and direction
void getReadings() {
  Speed = incomingSpeed; 
  if (isnan(Speed)) {
    Serial.println("Failed to read");
    Speed = 0.0;
  }

  Direction = incomingDir;
  if (isnan(Direction)) {
    Serial.println("Failed to read");
    Direction = 0.0;
  }
}

void printIncomingReadings() {
  // Display readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Speed: ");
  Serial.print(incomingSpeed);
  Serial.println(" units");
  Serial.print("Direction: ");
  Serial.print(incomingDir);
  Serial.println(" sign");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(L_PWM_motor_1, OUTPUT);
  pinMode(R_PWM_motor_1, OUTPUT);
  pinMode(L_PWM_motor_2, OUTPUT);
  pinMode(R_PWM_motor_2, OUTPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Register callback for Send Status
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time values were updated
    previousMillis = currentMillis;

    // Get incoming data
    getReadings();

    // Set values to send
    Data_structure.speed = Speed;
    Data_structure.dir = Direction;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &Data_structure, sizeof(Data_structure));

    // Print incoming readings and control motors
    if (debug_mode == 1) {
      Serial.println("------------------------ Debug Mode ----------------------");
      Serial.println("------------------------ Incoming Data ----------------------");
      printIncomingReadings();
      control();
      Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    }
  }
}

// Control the motors
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
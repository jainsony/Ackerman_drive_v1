// I am Transmitter 
// This micro-controller MAC: {0x3C, 0x61, 0x05, 0xD5, 0xD4, 0xC1}

#include <ESP8266WiFi.h>
#include <espnow.h>

// Other controller MAC
uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0xD9, 0x24, 0xDE};

uint8_t debug_mode = 1;
// Digital pin connected to the DHT sensor



#define buttonPin 5   // D4 digital

#define datalen 4
#define threshold 5

int i = 0;

int a_speed = 0;
int b_speed = 0;

// Define variables to store DHT readings to be sent
float Speed;
float Direction;

String dataString = ""; // variable to hold input data
int dataArray[datalen]; // array to hold parsed integers
int dataIndex = 0; // index for dataArray

// Define variables to store incoming readings
float FeedBackSpeed;
float FeedBackDirection;

// Updates DHT readings every 10 seconds
const long interval = 100; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float speed;
    float dir;
} struct_message;

// Create a struct_message called Data_structure to hold sensor readings
struct_message Data_structure;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  if (debug_mode == 1)
  {
    Serial.print("Bytes received: ");
    Serial.println(len);
  }
  FeedBackSpeed = incomingReadings.speed;
  FeedBackDirection = incomingReadings.dir;
}

// ##edit this function
void getReadings(){ // To get data from another esp
  // Read Speed

  Speed = 50; // ##replace this from speed 

  // Read Speed as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  if (isnan(Speed)){
    Serial.println("Failed to read from DHT");
    Speed = 0.0;
  }

  Direction = 1;  // ##replace this from direction

  if (isnan(Direction)){
    Serial.println("Failed to read from DHT");
    Direction = 0.0;
  }
}

void printIncomingReadings(){
  // Display Readings in Serial Monitor
  Serial.println("FEEDBACK READINGS");
  Serial.print("Speed: ");
  Serial.print(FeedBackSpeed);
  Serial.println(" units");
  Serial.print("Direction: ");
  Serial.print(FeedBackDirection);
  Serial.println(" sign");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Init DHT sensor
  // dht.begin();
  Serial.println("I am transmiting to : MAC: ec:fa:bc:5f:90:52");
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  pinMode(buttonPin, OUTPUT);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }


  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
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


void loop() {
  unsigned long currentMillis = millis();
  get_serial_reading();
  if (currentMillis - previousMillis >= interval) 
    {
      // save the last time you updated the DHT values
      previousMillis = currentMillis;

      //Get DHT readings
      // getReadings();
      get_serial_reading();

      //Set values to send
      Data_structure.speed = dataArray[0];
      Data_structure.dir = dataArray[1];


      // Send message via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &Data_structure, sizeof(Data_structure));

      // Print incoming readings
      if(debug_mode == 1)
      {
        Serial.println("------------------------ Debug Mode ----------------------");
        Serial.println("------------------------ FeedBack Data ----------------------");
        printIncomingReadings();
        Serial.println("------------------------ Paresed Data ----------------------");
        printData();
        Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
      }
    }

}


  //////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////



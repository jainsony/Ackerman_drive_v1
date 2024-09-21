#include <Servo.h>

#define DATALEN 4   // Expected length of data array
#define THRESHOLD 5 // Example threshold value

Servo x_axis_servo;

char dataBuffer[32];  // Buffer to hold incoming data
int dataArray[DATALEN]; // Array to hold parsed integers
int dataIndex = 0; // Index for dataArray
bool dataReady = false; // Flag to signal when data is fully received

int x_val;
int val;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baudrate

  x_axis_servo.attach(9);

  memset(dataArray, 0, sizeof(dataArray)); // Initialize dataArray with zeros
}

void loop() {
  if (Serial.available()) {
    char incomingChar = Serial.read();

    // Start and end markers for incoming data
    if (incomingChar == '<') {
      dataIndex = 0; // Reset index for new data set
      memset(dataBuffer, 0, sizeof(dataBuffer)); // Clear the buffer
    }
    else if (incomingChar == '>') {
      dataBuffer[dataIndex] = '\0'; // Null-terminate the buffer
      dataReady = true; // Signal that we have a full data set
    }
    else if (dataIndex < sizeof(dataBuffer) - 1) {
      // Only add valid characters within buffer limits
      dataBuffer[dataIndex++] = incomingChar;
    }
  }

  // Once data is received, parse and control
  if (dataReady) {
    parseData(); // Parse incoming data
    control();   // Execute control logic based on parsed data
    dataReady = false; // Reset flag for the next data set
  }
}

void parseData() {
  // Tokenize the buffer based on commas and convert to integers
  char* token = strtok(dataBuffer, ",");
  int index = 0;

  while (token != NULL && index < DATALEN) {
    dataArray[index++] = atoi(token); // Convert token to integer
    token = strtok(NULL, ","); // Move to the next token
  }

  // If we received less than the expected number of values, fill the rest with zeros
  while (index < DATALEN) {
    dataArray[index++] = 0;
  }
}

void control() {
  // Debugging: Print the parsed data for verification
  printData();

  x_val = dataArray[0];
  val = map(x_val, 0, 1023, 0, 180);
  x_axis_servo.write(val);
  Serial.print("Servo response");
  delay(10); 
  
}

void printData() {
  Serial.print("Parsed data: ");
  for (int i = 0; i < DATALEN; i++) {
    Serial.print(dataArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}

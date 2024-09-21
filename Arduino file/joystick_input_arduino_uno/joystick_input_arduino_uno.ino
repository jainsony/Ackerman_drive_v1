#include <Servo.h>

// Define the pin connections
int VRY_PIN = A0; // Potentiometer VRX pin connected to A0 (analog pin)
int VRX_PIN = A1; // Potentiometer VRY pin connected to digital pin D1
int SW_PIN = A2;  // Switch connected to digital pin D2
int EYE_POT = A3;

Servo left_upper_lid;
Servo left_lower_lid;
Servo right_upper_lid;
Servo right_lower_lid;
Servo x_axis_view;
Servo y_axis_view;

int vrxValue;
int vryValue;
int eyePotValue;

int x_axis_write_value;
int y_axis_write_value;
int eye_pot_write_value;

int eye_lid_value;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  left_upper_lid.attach(9);
  left_lower_lid.attach(8);
  right_upper_lid.attach(11);
  right_lower_lid.attach(10);
  x_axis_view.attach(6);
  y_axis_view.attach(7);

  // Initialize the switch pin as an input with pull-up resistor
  pinMode(SW_PIN, INPUT_PULLUP); 

  // Initialize the potentiometer pins (analog read pin doesn't need pinMode)
  pinMode(VRY_PIN, INPUT); // Digital pin for VRY
  pinMode(VRX_PIN, INPUT);
  pinMode(EYE_POT, INPUT);

  left_upper_lid.write(90);
  left_lower_lid.write(90);
  right_upper_lid.write(90);
  right_lower_lid.write(90);
  x_axis_view.write(90);
  y_axis_view.write(75);
}

void loop() {
  // Read the analog values from the potentiometers
  vrxValue = analogRead(VRX_PIN);  // Read VRX (A0 pin)
  vryValue = analogRead(VRY_PIN); // Read VRY (D1 pin, digital)
  eyePotValue = analogRead(EYE_POT);

  x_axis_write_value = map(vrxValue, 0, 1023, 60, 120);
  y_axis_write_value = map(vryValue, 0, 1023, 55, 95);
  eye_lid_value = map(y_axis_write_value,75, 95, 90, 150);
  eye_pot_write_value = map(eyePotValue, 0, 1023, 0, 40);
 
  Serial.print("EYE_POT : ");
  Serial.print(eye_pot_write_value);  
  Serial.print(" | x_axis : ");
  Serial.print(x_axis_write_value);
  Serial.print(" | y_axis : ");
  Serial.print(y_axis_write_value);
  Serial.print(" | upper_lid : ");
  Serial.print(eye_lid_value);
  Serial.print(" | upper_lid_offset : ");
  Serial.print(90 - (eye_lid_value - 90));



  // Read the switch state (LOW when pressed, HIGH when not pressed)
  int swState = digitalRead(SW_PIN);

  // Print the values to the Serial Monitor
  Serial.print(" |  VRX: ");
  Serial.print(vrxValue);
  Serial.print(" | VRY: ");
  Serial.print(vryValue);


  Serial.print(" | Switch: ");
  if (swState == LOW) {
    Serial.println("Pressed");
    delay(20);
    left_upper_lid.write(80);
    left_lower_lid.write(100);
    right_upper_lid.write(100);
    right_lower_lid.write(80);
    delay(200);

  } else {
    Serial.println("Not Pressed");
  }

  // Small delay to avoid too much serial output
  delay(20);

  all_servo_write();
}

void all_servo_write()
{
  // UPPER EYE LID
  if (y_axis_write_value > 75 and eye_lid_value > 120)
  {
    left_upper_lid.write(eye_lid_value);
    right_upper_lid.write(90 - (eye_lid_value - 90));
  }
  else
  {
    left_upper_lid.write(90 + eye_pot_write_value);
    right_upper_lid.write(90 - eye_pot_write_value);
  }

  // LOWER EYE LID
    if (y_axis_write_value < 75 and eye_lid_value < 60)
  {
    left_lower_lid.write(eye_lid_value);
    right_lower_lid.write(90 - (eye_lid_value - 90));
  }
  else
  {
    left_lower_lid.write(90 - eye_pot_write_value);
    right_lower_lid.write(90 + eye_pot_write_value);
  }

  x_axis_view.write(x_axis_write_value);
  y_axis_view.write(y_axis_write_value);
}

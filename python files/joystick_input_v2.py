import serial
import time
import sys
from inputs import get_gamepad, UnpluggedError

# Initialize the joystick control variables
x_axis = 100
y_axis = 200
blink = 300
shrink = 400

# Preformat the control string template to avoid repeated string concatenations
control_string_template = "<{},{},{},{}>\n"

# Attempt to initialize serial communication
try:
    ser = serial.Serial(port='COM10', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                        bytesize=serial.EIGHTBITS, timeout=0)
    print("Serial communication established.")
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    sys.exit(1)

# Set a threshold for deadzone handling
DEADZONE_THRESHOLD = 0.05

# Store the initial state of axes and buttons
axes_state = {"ABS_X": 0, "ABS_Y": 0, "ABS_RZ": 0}  # Default axes for x, y, shrink (z-rotation)
button_state = {"BTN_SOUTH": 0}  # Default button for "blink"

# Function to update axis values with deadzone handling
def update_axis_state(event):
    value = event.state / 32768.0  # Normalizing to a range of -1 to 1
    if abs(value) < DEADZONE_THRESHOLD:
        value = 0
    axes_state[event.code] = round(value, 2)

# Function to update button states
def update_button_state(event):
    button_state[event.code] = event.state

try:
    while True:
        # Fetch gamepad events
        try:
            events = get_gamepad()
        except UnpluggedError:
            print("No joystick detected. Please connect a joystick.")
            time.sleep(1)
            continue

        for event in events:
            # Handle axis events
            if event.ev_type == "Absolute" and event.code in axes_state:
                update_axis_state(event)

            # Handle button events
            elif event.ev_type == "Key" and event.code in button_state:
                update_button_state(event)

        # Map axes and button states to control variables
        x_axis = axes_state["ABS_X"]
        y_axis = -axes_state["ABS_Y"]
        shrink = -axes_state["ABS_RZ"]
        blink = button_state["BTN_SOUTH"]

        # Efficiently format the control string
        control_string = control_string_template.format(x_axis, y_axis, blink, shrink)

        # Send data over serial with error handling
        try:
            ser.write(control_string.encode())
        except serial.SerialException as e:
            print(f"Serial communication error: {e}")
            time.sleep(1)  # Pause before retrying
            continue

        # Debug print statement for joystick status and control string
        print(f"Axes: {axes_state} | Buttons: {button_state} | Control String: {control_string}")

        # Limit loop speed (e.g., 100 Hz)
        time.sleep(0.01)

except KeyboardInterrupt:
    print("Exiting joystick test.")
finally:
    # Clean up and close serial communication
    if ser.is_open:
        ser.close()
        print("Serial port closed.")

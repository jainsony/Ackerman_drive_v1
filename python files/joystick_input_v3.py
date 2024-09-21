import pygame
import serial
import time

# Initialize pygame and the joystick
pygame.init()
pygame.joystick.init()

# Function to map joystick values from [-1, 1] to [0, 1024]
def map_value(value, in_min, in_max, out_min, out_max):
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

# Pre-fetch the number of axes and buttons to avoid recalculating it in the loop
DEADZONE_THRESHOLD = 1e-4

# Use a clock to control the loop speed and prevent unnecessary high CPU usage
clock = pygame.time.Clock()

ser = serial.Serial(port='COM6', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=0)

# Check if a joystick is connected
if pygame.joystick.get_count() < 1:
    print("No joystick detected. Please connect a joystick.")
    exit()

# Create joystick object for the first connected joystick
joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Joystick Name: {joystick.get_name()}")

num_axes = joystick.get_numaxes()
num_buttons = joystick.get_numbuttons()

try:
    while True:
        pygame.event.pump()  # Process event queue (necessary to update joystick states)

        # Capture axes values and replace small floating-point noise with 0
        axes = [0 if abs(joystick.get_axis(i)) < DEADZONE_THRESHOLD else joystick.get_axis(i) for i in range(num_axes)]

        # Capture buttons states
        buttons = [joystick.get_button(i) for i in range(num_buttons)]

        # Map joystick values to [0, 1024]
        x_axis = map_value(round(axes[0], 2), -1, 1, 0, 1024)
        y_axis = map_value(-1 * round(axes[1], 2), -1, 1, 0, 1024)
        shrink = map_value(-1 * round(axes[3], 2), -1, 1, 0, 1024)
        blink = buttons[1]

        # Format and send the control string
        control_string = "<" + str(int(x_axis)) + "," + str(int(y_axis)) + "," + str(int(shrink)) + "," + str(blink) + ">\n"
        ser.write(str.encode(control_string))

        # Print the axes and buttons state in a single print statement for efficiency
        print(f"Axes: {axes} | Buttons: {buttons} | Control_String: {control_string}")

        # Limit the loop to run at a reasonable speed (e.g., 200 times per second)
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Exiting joystick test.")

finally:
    # Clean up and close pygame
    pygame.joystick.quit()
    pygame.quit()

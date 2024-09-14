import pygame
import serial
import time 

# Initialize pygame and the joystick
pygame.init()
pygame.joystick.init()

x_axis = 100
y_axis = 200
blink = 300
shrink = 400

control_string = "<"+str(x_axis)+","+ str(y_axis)+","+str(blink)+","+str(shrink)+">"+"\n"

ser = serial.Serial(port='COM10',baudrate=115200,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=0)

# Check if a joystick is connected
if pygame.joystick.get_count() < 1:
    print("No joystick detected. Please connect a joystick.")
    exit()

# Create joystick object for the first connected joystick
joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Joystick Name: {joystick.get_name()}")

# Pre-fetch the number of axes and buttons to avoid recalculating it in the loop
num_axes = joystick.get_numaxes()
num_buttons = joystick.get_numbuttons()

# Set a threshold for floating-point errors
DEADZONE_THRESHOLD = 1e-4

# Use a clock to control the loop speed and prevent unnecessary high CPU usage
clock = pygame.time.Clock()

try:
    while True:
        pygame.event.pump()  # Process event queue (necessary to update joystick states)

        # Capture axes values and replace small floating-point noise with 0
        axes = [0 if abs(joystick.get_axis(i)) < DEADZONE_THRESHOLD else joystick.get_axis(i) for i in range(num_axes)]

        # Capture buttons states
        buttons = [joystick.get_button(i) for i in range(num_buttons)]

        x_axis = round(axes[0], 2)
        y_axis = -1*round(axes[1], 2)
        shrink = -1*round(axes[3], 2)
        blink = buttons[1]


        # Print the axes and buttons state in a single print statement for efficiency
        print(f"Axes: {axes} | Buttons: {buttons}")
        control_string = "<"+str(x_axis)+","+ str(y_axis)+","+str(blink)+","+str(shrink)+">"+"\n"
        ser.write(str.encode(control_string))

        # Limit the loop to run at a reasonable speed (e.g., 200 times per second)
        # clock.tick(200)
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Exiting joystick test.")

# finally:
#     # Clean up and close pygame
#     pygame.joystick.quit()
#     pygame.quit()

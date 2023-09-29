import serial
import matplotlib.pyplot as plt
from collections import deque

# Initialize serial connection to Arduino
arduino = serial.Serial('COM4', 9600)  # Change 'COM3' to your Arduino's serial port

# Initialize a deque to store the data
data = deque(maxlen=100)  # Keep a fixed number of data points for the plot

# Create the plot
plt.ion()  # Turn on interactive mode
fig, ax = plt.subplots()
line, = ax.plot([], '-', label='Sensor Data')
ax.set_title('Live Data from Arduino')
ax.set_xlabel('Time')
ax.set_ylabel('Sensor Reading')
ax.grid(True)
ax.legend(loc='upper left')

# Set x and y-axis limits
ax.set_xlim(0, 100)  # Adjust the limits as needed
ax.set_ylim(-100, 100)  # Adjust the limits as needed

try:
# Main loop to read and update data
    while True:
        # Read data from Arduino
        sensor_value = float(arduino.readline().decode().strip())
        data.append(sensor_value)

        # Update the plot data
        line.set_data(range(len(data)), data)

        # Redraw the plot
        ax.relim()
        ax.autoscale_view()
        plt.draw()
        plt.pause(0.1)  # Pause briefly to update the plot

except KeyboardInterrupt:
    print("Closing the plot and serial port...")
    arduino.close()  # Close the serial port
    plt.ioff()  # Turn off interactive mode
    plt.close()  # Close the plot window
import time
from pyftdi.i2c import I2cController

# Initialize the FT232H as an I2C controller
controller = I2cController()

# Open the first available interface (assuming FT232H is connected)
controller.configure('ftdi://ftdi:ft232h/1')

# Define the I2C addresses you want to communicate with
address = 0x01   # Example addresses

# Read data asked from the i2c device
data = controller.read(address, 4)
print(data)

# have a switch case with the possible adresses it asks for
toSend = 0x00
if data == 0x02:
    toSend = 0x01
elif data == 0x03:
    toSend = 0x02
elif data == 0x04:
    toSend = 0x03


# Send data to the I2C device
controller.write(address, [toSend])

# Close the I2C interface




# Process the data as needed


   
    # Add any processing or logic as needed
    # You can also write data to the addresses if required

# Close the I2C interface
controller.terminate()
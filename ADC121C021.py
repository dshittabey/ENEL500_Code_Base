  
# Distributed with a free-will license.
# Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
# ADC121C021
# This code is designed to work with the ADC121C021_I2CADC I2C Mini Module available from ControlEverything.com.
# https://www.controleverything.com/content/Analog-Digital-Converters?sku=ADC121C021_I2CADC#tabs-0-product_tabset-2

import smbus
import time
import datetime





def send_to_file(buffer_list,filename):
    length = len(buffer_list)
    count = 0
    with open(filename, 'w') as f:
        for list_item in buffer_list:
            if count != length-1:
                f.write('%s,\n'%list_item)
            else:
                f.write('%s' % list_item)
            count += 1
    return


# Get I2C bus
bus = smbus.SMBus(1)

# ADC121C021 address, 0x50(80)
# Select configuration register, 0x02(02)
#		0x20(32)	Automatic conversion mode enabled
bus.write_byte_data(0x50, 0x02, 0x20)

time.sleep(0.5)
voltage_buffer = []
time_buffer = []
samples_per_sec = 450*60
sample_rate = 1/samples_per_sec
# ADC121C021 address, 0x50(80)
# Read data back from 0x00(00), 2 bytes
# raw_adc MSB, raw_adc LSB
#data = bus.read_i2c_block_data(0x50, 0x00, 2)
#raw_adc = (data[0] & 0x0F) * 256 + data[1]
#voltage = raw_adc * (5/4095)

top = time.time()
# Convert the data to 12-bits
for i in range(samples_per_sec):
    top = time.time()
    data = bus.read_i2c_block_data(0x50, 0x00, 2)
    raw_adc = (data[0] & 0x0F) * 256 + data[1]
    voltage = raw_adc * (5/4095)
    voltage_buffer.append(voltage)
#   current_time = datetime.datetime.now().strftime("%H:%M:%S.%f")
#    time_buffer.append(current_time)
    time.sleep(sample_rate)
    bottom = time.time()

print("Time taken to convert:{}us".format((bottom-top)*1e6))
send_to_file(voltage_buffer,"test.txt")    

# Output data to screen
#print(voltage_buffer)
print()
print()
print()
print()
#print(time_buffer)
print()


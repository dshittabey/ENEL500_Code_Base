  
// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ADC121C021
// This code is designed to work with the ADC121C021_I2CADC I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Analog-Digital-Converters?sku=ADC121C021_I2CADC#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>

typedef struct timespec TIMESPEC;

// This choice of clock seems to give the highest resolution on macOS.
#define WHICH_CLOCK CLOCK_MONOTONIC_RAW
#define THRESHOLD_BYTES_TO_READ 6750
#define SAMPLES	27000

double timespec2ns(const TIMESPEC *ts)
{
  return 1.0e9 * ts->tv_sec + ts->tv_nsec;
}



int main()
{
	TIMESPEC start, finish;
	double polling_time;
	double total_time = 0;
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, ADC121C021 I2C address is 0x50(80)
	ioctl(file, I2C_SLAVE, 0x50);

	// Select configuration register(0x02)
	// Automatic conversion mode enabled(0x20)
	char config[2] = {0};
	config[0] = 0x02;
	config[1] = 0x20;
	write(file, config, 2);
	sleep(1);
	// Note the start time.
  	clock_gettime(WHICH_CLOCK, &start);
	// Read 2 bytes of data from register(0x00)
	// raw_adc msb, raw_adc lsb
	char reg[1] = {0x00};
	write(file, reg, 1);

	char data[THRESHOLD_BYTES_TO_READ] = {0};
        int raw_adc_array[SAMPLES] = {0};
	double time_axis[SAMPLES] = {0};
	int counter = 0;

	while(counter < SAMPLES){
	clock_gettime(WHICH_CLOCK, &start);
	if(read(file, data, THRESHOLD_BYTES_TO_READ) != THRESHOLD_BYTES_TO_READ)
	{
		printf("Error : Input/Output Error \n");

	}
	else
	{
             for(int i = 0; i < THRESHOLD_BYTES_TO_READ; i = i+2){
		// Convert the data to 12-bits
		int raw_adc = ((data[i] & 0x0F) * 256) + data[i+1];
  		raw_adc_array[counter] = raw_adc;
		//time_axis[counter] = polling_time;
		counter++;

		//total_time += polling_time;
		// Output data to screen
		//float volts = (raw_adc)*5.0/4095.0;
		//printf("Voltage Value of #%d Analog Input : %f \nPolling time: %.2f us\n\n", counter,volts, polling_time*1e-3);

		}
	clock_gettime(WHICH_CLOCK, &finish);
        polling_time = timespec2ns(&finish) - timespec2ns(&start);
	total_time += polling_time;
        //printf("Time to get 6400 bytes: %.2fus\n", polling_time*1e-3);

	}
}
	printf("Total time taken to get 27000 samples: %.2fs\n", total_time*1e-9);
	return 0;
}

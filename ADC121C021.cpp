// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ADC121C021
// This code is designed to work with the ADC121C021_I2CADC I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Analog-Digital-Converters?sku=ADC121C021_I2CADC#tabs-0-product_tabset-2

#include "header.h"



// This choice of clock seems to give the highest resolution on macOS.
#define WHICH_CLOCK CLOCK_MONOTONIC_RAW
#define SAMPLES	32768 //Samples per second
#define THRESHOLD_BYTES_TO_READ SAMPLES/4
#define MAXIMUM 1048576 // Maximum value going into fft program

double timespec2ns(const TIMESPEC *ts)
{
  return 1.0e9 * ts->tv_sec + ts->tv_nsec;
}

//Writes to a file that is already initialized with file handle f
void write2txtfile(FILE *f, float value){
    if (f == NULL){
        printf("Error opening file!\n");
        return;
    }
    fprintf(f, "%f\n", value);    
}


int main(){
	TIMESPEC start, finish;
    //read_start_time, read_done_time;
	double polling_time;
	double total_time = 0;
	// Create I2C bus
	int file;
	const char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0){
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
    //int raw_adc_array[SAMPLES] = {0};
	//double time_axis[SAMPLES] = {0};
	int counter = 0;
    int raw_adc;
    float volts_array[SAMPLES] = {0};
    //double reading_time;
	while(counter < SAMPLES){
        clock_gettime(WHICH_CLOCK, &start);
       // clock_gettime(WHICH_CLOCK, &read_start_time);
        if(read(file, data, THRESHOLD_BYTES_TO_READ)!= THRESHOLD_BYTES_TO_READ) printf("Error : Input/Output Error \n");
       
       // clock_gettime(WHICH_CLOCK, &read_done_time);
       //reading_time = timespec2ns(&read_done_time) - timespec2ns(&read_start_time);
        else{
            for(int i = 0; i < THRESHOLD_BYTES_TO_READ; i = i+2){
                // Convert the data to 12-bits
                raw_adc = ((data[i] & 0x0F) * 256) + data[i+1];
                //raw_adc_array[counter] = raw_adc;
                float volts = (raw_adc)*5.0/4095.0;
                volts_array[counter] = volts;
                
                //time_axis[counter] = polling_time;
                counter++;

                //total_time += polling_time;
                // Output data to screen
                
                //printf("Voltage Value of #%d Analog Input : %f \nPolling time: %.2f us\n\n", counter,volts, polling_time*1e-3);

            }
            //printf("Time to get 6400 bytes: %.2fus\n", polling_time*1e-3);
            clock_gettime(WHICH_CLOCK, &finish);
            polling_time = timespec2ns(&finish) - timespec2ns(&start);
            total_time += polling_time;
            printf("Time to read raw %d bytes: %.2fus\n",THRESHOLD_BYTES_TO_READ, polling_time*1e-3);
        }
        
    }
	printf("\nTotal time taken to get %d samples: %.2fms\n", SAMPLES, total_time*1e-6);
    
    
    
    FILE *f = fopen("voltc.txt", "w");
    //FILE *t = fopen("timec.txt", "w"); 
    for(int i = 0; i<SAMPLES; i++){
        write2txtfile(f, volts_array[i]);    
    }    
    
	return 0;
}

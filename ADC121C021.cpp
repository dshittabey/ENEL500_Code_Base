#include "header.h"
#include <typeinfo>




// This choice of clock seems to give the highest resolution on macOS.
#define WHICH_CLOCK CLOCK_MONOTONIC_RAW
#define SAMPLES	32768 //Samples per second
#define FFT_PERIOD 32 //taking 32.something seconds of samples
#define THRESHOLD_BYTES_TO_READ SAMPLES/4
#define WEEKDAY_START 0
#define WEEKDAY_END 2
#define MONTH_START 4
#define MONTH_END 6
#define DAY_START 8
#define DAY_END 9
#define HOUR_START 11
#define HOUR_END 12
#define MINUTE_START 14
#define MINUTE_END 15
#define SECOND_START 17
#define SECOND_END 18
#define YEAR_START 20
#define YEAR_END 23
#define CIRCUIT_GAIN 8.5

int file;
double volts_array[FFT_PERIOD*SAMPLES] = {0};
double InputI[FFT_PERIOD*SAMPLES] = {0};
char data[THRESHOLD_BYTES_TO_READ] = {0};


// Returns time in nanoseconds
double timespec2ns(const TIMESPEC *ts){
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



void start_polling(){
    TIMESPEC start, finish;
	double polling_time;
	double total_time = 0;
	int counter = 0;
    int raw_adc;
	while(counter < SAMPLES){
        clock_gettime(WHICH_CLOCK, &start);
        if(read(file, data, THRESHOLD_BYTES_TO_READ)!= THRESHOLD_BYTES_TO_READ) printf("Error : Input/Output Error \n");

        else{
            for(int i = 0; i < THRESHOLD_BYTES_TO_READ; i = i+2){    
                raw_adc = ((data[i] & 0x0F) * 256) + data[i+1]; // Convert the data to 12-bits
                //volts_array[counter] = (raw_adc)*5.0/4095.0;
				volts_array[counter] = (((raw_adc)*5.0/4095.0) - 2.5)*CIRCUIT_GAIN;
                counter++;
            }
            clock_gettime(WHICH_CLOCK, &finish);
            polling_time = timespec2ns(&finish) - timespec2ns(&start);
            total_time += polling_time;
            printf("Time to read raw %d bytes: %.2fus\n",THRESHOLD_BYTES_TO_READ, polling_time*1e-3);
        }
        
    }
	printf("Total time taken to get %d samples: %.2fs\n\n", SAMPLES, total_time*1e-9);   
}

//static volatile bool keep_running = true;

//static void* userInput_thread(void*){
//    while(keep_running) {
 //       if (std::cin.get() == 'q'){
 //           //! desired user input 'q' received
 //          keep_running = false;
 //       }
//    }
 //   pthread_exit(NULL);
//}

// Some parts of this function were obtained from
// https://www.controleverything.com/content/Analog-Digital-Converters?sku=ADC121C021_I2CADC#tabs-0-product_tabset-2
// Create I2C bus
void initialize_i2c_communication(){
	const char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0){
		printf("Failed to open the bus. \n");
		exit(1);
	}
	
	ioctl(file, I2C_SLAVE, 0x50); // Get I2C device, ADC121C021 I2C address is 0x50(80)
	
	char config[2] = {0};
	config[0] = 0x02; // Select configuration register(0x02)
	config[1] = 0x20; // Automatic conversion mode enabled(0x20 = 0010_0000)
	write(file, config, 2);
	sleep(1);
    char reg[1] = {0x00}; 
	write(file, reg, 1);  //Select conversion register (0x00) of ADC121C021 in order to start reading converted result
}

// Splits current date time into seperate strings and stores them in string variable of choice
std::string 
date_param_splitter(int begin, int end, char * date_array){
    std::string date_param = "";
    for(int i = begin;i<=end;i++){
        date_param.push_back(date_array[i]); 
    }
    return date_param;
}

std::string 
get_second(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(SECOND_START,SECOND_END,date_time_pointer);
}



std::string 
get_minute(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(MINUTE_START,MINUTE_END,date_time_pointer);
}

std::string 
get_hour(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(HOUR_START,HOUR_END,date_time_pointer);
}

std::string 
get_day(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(DAY_START,DAY_END,date_time_pointer);
}

std::string 
get_weekday(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(WEEKDAY_START,WEEKDAY_END,date_time_pointer);
}

std::string 
get_month(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(MONTH_START,MONTH_END,date_time_pointer);
}

std::string 
get_year(){
    time_t current_time;
    char *date_time_pointer;
    time(&current_time);
    date_time_pointer = ctime(&current_time);
    return date_param_splitter(YEAR_START,YEAR_END,date_time_pointer);
}

void print_current_date_and_time() { 
    time_t curtime;  
    time(&curtime);  
    printf("Current date and time (24-hour clock) --> %s\n\n", ctime(&curtime)); 
} 


int main(){
    print_current_date_and_time();
    printf("\nPolling process starting... \nTo quit process anytime press 'q' then press enter...\n\n");
    initialize_i2c_communication();  
   // pthread_t tId;
   // (void) pthread_create(&tId, 0, userInput_thread, 0);
    
    std::string current_minute = get_minute();
   // while(current_minute == get_minute()){
        std::string current_second = get_second();
                   
        int number_of_samples = 0;
     //   for(int i = 0; i<32; i++){
            start_polling();
            number_of_samples += SAMPLES;
     //   }    
        //FFT(volts_array, InputI, number_of_samples, FORWARD);
        //Magnitude(volts_array, InputI, number_of_samples);
		std::string end_minute = get_minute();
		std::string end_second = get_second();
        std::string file_name = "Time_File_for_min"+current_minute+"_"+"sec"+current_second+"_to_"+"min"+end_minute+"_sec"+end_second+".txt";
        std::ofstream Time_File(file_name,std::ios::app);
		
        for(int i = 0; i<number_of_samples; i++){
            if (Time_File.is_open()) Time_File << volts_array[i] << std::endl;
        }   
        Time_File.close();
  //  }
    
    //pthread_exit(NULL);
    printf("\nPolling process ending...\n");
    
	return 0;
}
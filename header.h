#ifndef header_h
#define header_h


#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string> 
#include <sys/types.h> 
#include <sys/stat.h> 
#define M_2PI  6.28318530717958647692
#define MAXIMUM_FFT_SIZE  1048576
#define MINIMUM_FFT_SIZE  8



typedef struct timespec TIMESPEC;
enum TTransFormType {FORWARD, INVERSE};
void initialize_i2c_communication();
void print_current_date_and_time();
void start_polling();
//static void* userInput_thread(void*);
int RequiredFFTSize(int NumPts);
int IsValidFFTSize(int x);
void FFT(double *InputR, double *InputI, int N, TTransFormType Type);
void ReArrangeInput(double *InputR, double *InputI, double *BufferR, double *BufferI, int *RevBits, int N);
void FillTwiddleArray(double *TwiddleR, double *TwiddleI, int N, TTransFormType Type);
void Transform(double *InputR, double *InputI, double *BufferR, double *BufferI, double *TwiddleR, double *TwiddleI, int N);
double timespec2ns(const TIMESPEC *ts);
void write2txtfile(FILE *f, float value);
void Magnitude(double *InputR, double *InputI, int N);
int IsValidFFTSize(int x);
#endif
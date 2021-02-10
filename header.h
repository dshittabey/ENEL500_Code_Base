#ifndef header_h
#define header_h


#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

typedef struct timespec TIMESPEC;
enum TTransFormType {FORWARD, INVERSE};
int RequiredFFTSize(int NumPts);
int IsValidFFTSize(int x);
void FFT(double *InputR, double *InputI, int N, TTransFormType Type);
void ReArrangeInput(double *InputR, double *InputI, double *BufferR, double *BufferI, int *RevBits, int N);
void FillTwiddleArray(double *TwiddleR, double *TwiddleI, int N, TTransFormType Type);
void Transform(double *InputR, double *InputI, double *BufferR, double *BufferI, double *TwiddleR, double *TwiddleI, int N);
double timespec2ns(const TIMESPEC *ts);
void write2txtfile(FILE *f, float value);

#endif
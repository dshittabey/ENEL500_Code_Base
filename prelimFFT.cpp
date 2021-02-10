#include "header.h"

// The Fourier Transform.
void Transform(double *InputR, double *InputI, double *BufferR, double *BufferI, double *TwiddleR, double *TwiddleI, int N)
{
 int j, k, J, K, I, T;
 double *TempPointer;
 double TempR, TempI;

 J = N/2;     // J increments down to 1
 K = 1;       // K increments up to N/2
 while(J > 0) // Loops Log2(N) times.
  {
   // Swap pointers, instead doing this: for(j=0; j<N; j++) Input[j] = Buffer[j];
   // We start with a swap because of the swap in ReArrangeInput.
   TempPointer = InputR;
   InputR = BufferR;
   BufferR = TempPointer;
   TempPointer = InputI;
   InputI = BufferI;
   BufferI = TempPointer;

   I = 0;
   for(j=0; j<J; j++)
	{
	 T = 0;
	 for(k=0; k<K; k++) // Loops N/2 times for every value of J and K
	  {
	   TempR = InputR[K+I] * TwiddleR[T] - InputI[K+I] * TwiddleI[T];
	   TempI = InputR[K+I] * TwiddleI[T] + InputI[K+I] * TwiddleR[T];
	   BufferR[I]   = InputR[I] + TempR;
	   BufferI[I]   = InputI[I] + TempI;
	   BufferR[I+K] = InputR[I] - TempR;
	   BufferI[I+K] = InputI[I] - TempI;
	   I++;
	   T += J;
	  }
	 I += K;
	}
   K *= 2;
   J /= 2;
  }

}

//-----------------------------------------------------------------------------------------------










/*
 The Pentium takes a surprising amount of time to calculate the sine and cosine.
 You may want to make the twiddle arrays static if doing repeated FFTs of the same size.
 This uses 4 fold symmetry to calculate the twiddle factors. As a result, this function
 requires a minimum FFT size of 8.
*/
void FillTwiddleArray(double *TwiddleR, double *TwiddleI, int N, TTransFormType Type)
{
 int j;
 double Theta, TwoPiOverN;

 TwoPiOverN = M_2PI / (double) N;

 if(Type == FORWARD)
  {
   TwiddleR[0] = 1.0;
   TwiddleI[0] = 0.0;
   TwiddleR[N/4] = 0.0;
   TwiddleI[N/4] = -1.0;
   TwiddleR[N/8] = M_SQRT_2;
   TwiddleI[N/8] = -M_SQRT_2;
   TwiddleR[3*N/8] = -M_SQRT_2;
   TwiddleI[3*N/8] = -M_SQRT_2;
   for(j=1; j<N/8; j++)
	{
	 Theta = (double)j * -TwoPiOverN;
	 TwiddleR[j] = cos(Theta);
	 TwiddleI[j] = sin(Theta);
	 TwiddleR[N/4-j] = -TwiddleI[j];
	 TwiddleI[N/4-j] = -TwiddleR[j];
	 TwiddleR[N/4+j] = TwiddleI[j];
	 TwiddleI[N/4+j] = -TwiddleR[j];
	 TwiddleR[N/2-j] = -TwiddleR[j];
	 TwiddleI[N/2-j] = TwiddleI[j];
	}
  }

 else
  {
   TwiddleR[0] = 1.0;
   TwiddleI[0] = 0.0;
   TwiddleR[N/4] = 0.0;
   TwiddleI[N/4] = 1.0;
   TwiddleR[N/8] = M_SQRT_2;
   TwiddleI[N/8] = M_SQRT_2;
   TwiddleR[3*N/8] = -M_SQRT_2;
   TwiddleI[3*N/8] = M_SQRT_2;
   for(j=1; j<N/8; j++)
	{
	 Theta = (double)j * TwoPiOverN;
	 TwiddleR[j] = cos(Theta);
	 TwiddleI[j] = sin(Theta);
	 TwiddleR[N/4-j] = TwiddleI[j];
	 TwiddleI[N/4-j] = TwiddleR[j];
	 TwiddleR[N/4+j] = -TwiddleI[j];
	 TwiddleI[N/4+j] = TwiddleR[j];
	 TwiddleR[N/2-j] = -TwiddleR[j];
	 TwiddleI[N/2-j] = TwiddleI[j];
	}
  }

}

//--------------------------------------------------------------------------







// This puts the input arrays in bit reversed order.
// The while loop generates an array of bit reversed numbers. e.g.
// N=8: 0,4,2,6,1,5,3,7  N=16: 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15
void ReArrangeInput(double *InputR, double *InputI, double *BufferR, double *BufferI, int *RevBits, int N)
{
 int j, k, J, K;

 J = N/2;
 K = 1;
 RevBits[0] = 0;
 while(J >= 1)
  {
   for(k=0; k<K; k++)
	{
	 RevBits[k+K] = RevBits[k] + J;
	}
   K *= 2;
   J /= 2;
  }


 // Move the rearranged input values to Buffer.
 // Take note of the pointer swaps at the top of the transform algorithm.
 for(j=0; j<N; j++)
  {
   BufferR[j] = InputR[ RevBits[j] ];
   BufferI[j] = InputI[ RevBits[j] ];
  }

}

//---------------------------------------------------------------------------

// Fast Fourier Transform
// This code puts DC in bin 0 and scales the output of a forward transform by 1/N.
// InputR and InputI are the real and imaginary input arrays of length N.
// The output values are returned in the Input arrays.
// TTransFormType is either FORWARD or INVERSE (defined in the header file)
// 256 pts in 50 us
void FFT(double *InputR, double *InputI, int N, TTransFormType Type)
{
 int j, LogTwoOfN, *RevBits;
 double *BufferR, *BufferI, *TwiddleR, *TwiddleI;
 double OneOverN;

 // Verify the FFT size and type.
 LogTwoOfN = IsValidFFTSize(N);
 if(LogTwoOfN == 0 || (Type != FORWARD && Type != INVERSE) )
 {
  ShowMessage("Invalid FFT type or size.");
  return;
 }

 // Memory allocation for all the arrays.
 BufferR  = new(std::nothrow) double[N];
 BufferI  = new(std::nothrow) double[N];
 TwiddleR = new(std::nothrow) double[N/2];
 TwiddleI = new(std::nothrow) double[N/2];
 RevBits  = new(std::nothrow) int[N];

 if(BufferR == NULL  || BufferI == NULL  ||
	TwiddleR == NULL || TwiddleI == NULL || RevBits == NULL)
  {
   ShowMessage("FFT Memory Allocation Error");
   return;
  }

 ReArrangeInput(InputR, InputI, BufferR, BufferI, RevBits, N);
 FillTwiddleArray(TwiddleR, TwiddleI, N, Type);
 Transform(InputR, InputI, BufferR, BufferI, TwiddleR, TwiddleI, N);


 // The ReArrangeInput function swapped Input[] and Buffer[]. Then Transform()
 // swapped them again, LogTwoOfN times. Ultimately, these swaps must be done
 // an even number of times, or the pointer to Buffer gets returned.
 // So we must do one more swap here, for N = 16, 64, 256, 1024, ...
 OneOverN = 1.0;
 if(Type == FORWARD) OneOverN = 1.0 / (double)N;

 if(LogTwoOfN % 2 == 1)
  {
   for(j=0; j<N; j++) InputR[j] = InputR[j] * OneOverN;
   for(j=0; j<N; j++) InputI[j] = InputI[j] * OneOverN;
  }
 else // if(LogTwoOfN % 2 == 0) then the results are still in Buffer.
  {
   for(j=0; j<N; j++) InputR[j] = BufferR[j] * OneOverN;
   for(j=0; j<N; j++) InputI[j] = BufferI[j] * OneOverN;
  }

 delete[] BufferR;
 delete[] BufferI;
 delete[] TwiddleR;
 delete[] TwiddleI;
 delete[] RevBits;
}
//---------------------------------------------------------------------------

// This puts the input arrays in bit reversed order.
// The while loop generates an array of bit reversed numbers. e.g.
// N=8: 0,4,2,6,1,5,3,7  N=16: 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15
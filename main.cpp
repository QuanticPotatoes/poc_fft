#include <iostream>
#include <fftw3.h>
#include <sndfile.h>
#include <cmath>
#include <stdlib.h>
#include "png.hpp"

int main(int argc, char ** argv) {

  SF_INFO       sfinfo ;
  SNDFILE       *infile;
  fftw_plan     p;
  int N = 512;
  int readcount ;
  int samplecount = 0;
  double * in;
  fftw_complex *out;
  int half = N/2;
  double *processed;
  PIXEL * img;
  int x = 0;
  int mfcc_mel = 60;
  unsigned int color;
  double * averageBuffer;
  double *mfccs;
  double *result_mfcc;


  if (! (infile = sf_open(argv[1], SFM_READ, &sfinfo))) {   /* Open failed so print an error message. */
    std::cout << "Impossible d'ouvrir le fichier d'entré...\n" ;
    sf_perror (NULL) ;
    return  1 ;
  } ; 

  if (sfinfo.channels > 2) {
    std::cout <<"Impossible de traiter plus de deux canaux...\n" ;
    return  1 ;
  } ;

  std::cout << "samples: " << sfinfo.frames << "\n";
  std::cout << "sample rate: " << sfinfo.samplerate << "\n";


  img = new PIXEL[half * (sfinfo.frames/half)];
 
  in = (double*) fftw_malloc(sizeof(double) * N);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  processed = (double*) fftw_malloc(sizeof(double) * N);
  mfccs = (double*) fftw_malloc(sizeof(double) * half * (sfinfo.frames/half));
  result_mfcc = (double*) fftw_malloc(sizeof(double) * (mfcc_mel * 256));

  p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
  
  while((readcount = sf_read_double(infile,in,N))){
  
  	fftw_execute(p); /* repeat as needed */
  	
   // std::cout << "Frequence :" << x*((44100)/N) << "\n";

  	for(int i = 0; i < N; i++) {

     out[i][0] *= N; // Real Values
     out[i][1] *= N; // Complex Values



     mfccs[x + i * (sfinfo.frames/N)] = pow(abs(out[i][1]),2); //absolute value of the complex fourier transform
     mfccs[x + i * (sfinfo.frames/N)] *= 1125 * log( (x*((44100)/N)/700 ) ) + 22; // multiply each complex value with each filterbank


     processed[i] = (sqrtf(out[i][0]*out[i][0] + out[i][1]*out[i][1])); // Power Spectrum
     processed[i] = 10./log(10.) * log(processed[i] + 1e-6); // dB
     
	   	if (processed[i] <= 0)
		   	{
		   		processed[i] = 0;
		   	}


		  color =  255*255 -(log (processed[i] + 1e-6)/log(10)) /-60. *255*255*128 ;

      //color = (int) ( 10 * ( log10( 2 * ( pow((processed[i] * 255 * 255 ) * ( 2 /  (double) N ) , 2) ))) / -60   );
      //color = (int) ( 10 * ( log10( 2 * ( pow((processed[i] * 255 ) ,2)))));
          //std::cout << x + sfinfo.frames - i * (sfinfo.frames/half) << "\n";

          img[x + i * (sfinfo.frames/N)].Red = (color & 255);
		      img[x + i * (sfinfo.frames/N)].Green = ((color & (255 << 8)) >> 8);
		      img[x + i * (sfinfo.frames/N)].Blue = ((color & (255 << 16)) >> 16);
		      img[x + i * (sfinfo.frames/N)].Alpha = 255;
    
      }
      x++;
  }


  int v = 0;

  for(int j = 0; j < 256; j++){
    for(int i = 300; i < 8000; i+= ((8000 - 300)/ mfcc_mel) )
    {

      result_mfcc[j + v * mfcc_mel] = mfccs[j + ((int)floor( (N+1) * i / sfinfo.samplerate )) * (sfinfo.frames/N)];

    }
    v++;
  }

  //mfccs[x + i * (sfinfo.frames/N)] *= 1125 * log( (x*((44100)/N)/700 ) ) + 22; // multiply each complex value with each filterbank


  ;


  writePng(img, (sfinfo.frames/N), half);
 
  // ...


  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
  sf_close (infile); 
  return 0;
}
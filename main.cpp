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
  unsigned int color;
  double * averageBuffer;

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

  p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
  
  while((readcount = sf_read_double(infile,in,N))){
  
  	fftw_execute(p); /* repeat as needed */
  	
  	for(int i = 0; i < N; i++) {

     out[i][0] *= half; // Real Values
     out[i][1] *= half; // Complex Values

     processed[i] = out[i][0]*out[i][0] + out[i][1]*out[i][1]; // Power Spectrum
     processed[i] = 10./log(10.) * log(processed[i] + 1e-6); // dB
   
	   	if (processed[i] <= 0)
		   	{
		   		processed[i] = 0;
		   	}

		  color =  (log (processed[i] + 1e-6)/log(10)) /-60. *255*255*128;
      //color = (int) ( 10 * ( log10( 2 * ( pow((processed[i] * 255 * 255 ) * ( 2 /  (double) N ) , 2) ))) / -60   );
      //color = (int) ( 10 * ( log10( 2 * ( pow((processed[i] * 255 ) ,2)))));
          //std::cout << x + sfinfo.frames - i * (sfinfo.frames/half) << "\n";
          img[x + i * (sfinfo.frames/N)].Red = (color & 255);
		      img[x + i * (sfinfo.frames/N)].Green = ((color & (255 << 8)) >> 8);
		      img[x + i * (sfinfo.frames/N)].Blue = ((color & (255 << 16)) >> 16);
		      img[x + i * (sfinfo.frames/N)].Alpha = 255;
    
      }
      x++;
  };


  writePng(img, (sfinfo.frames/N), half);
 
  // ...


  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
  sf_close (infile); 
  return 0;
}
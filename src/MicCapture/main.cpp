#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <time.h>
#include "pulse/simple.h"
#include "pulse/error.h"

#define BUFSIZE 1024

using namespace std;

namespace writeWavIO
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}

using namespace writeWavIO;

int main(int argc, char const *argv[])
{

	time_t t2,t1 = time(NULL);
	t2 = t1;

	// pulseaudio connection
	pa_simple *paconn = NULL;
	pa_sample_spec ss;

	ss.format = PA_SAMPLE_S16NE;
	ss.channels = 2;
	ss.rate = 44100;

	//wav file

	ofstream f("record.wav", ios::binary);
		  // Write the file headers
	  f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
	  
	  write_word( f,     16, 4 );  // no extension data
	  write_word( f,      1, 2 );  // PCM - integer samples
	  write_word( f,      2, 2 );  // two channels (stereo file)
	  write_word( f,  44100, 4 );  // samples per second (Hz)
	  write_word( f, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
	  write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
	  write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

		    // Write the data chunk header
	  size_t data_chunk_pos = f.tellp();
	  f << "data----";  // (chunk size to be filled in later)
	  
	uint8_t buf[BUFSIZE];
	ssize_t r;
	int error;

	paconn = pa_simple_new(NULL,
						"micrecord",
						PA_STREAM_RECORD,
						NULL,
						"microphone",
						&ss,
						NULL,
						NULL,
						NULL
						);


	while( (t2 - t1) < 3 ){

	pa_simple_read(paconn,buf,sizeof(buf),&error);

	for(uint8_t i : buf ){

		write_word(f,i);

	}

	t2 = time(NULL);

	}
	
	  // (We'll need the final file size to fix the chunk sizes above)
	  size_t file_length = f.tellp();

	  // Fix the data chunk header to contain the data size
	  f.seekp( data_chunk_pos + 4 );
	  write_word( f, file_length - data_chunk_pos + 8 );

	  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
	  f.seekp( 0 + 4 );
	  write_word( f, file_length - 8, 4 ); 


	pa_simple_free(paconn);


	return 0;
}

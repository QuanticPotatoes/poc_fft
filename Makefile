poc_fft:
	g++ *.cpp -lfftw3 -lsndfile -lm -lpng -Wno-write-strings

clean:
	rm -rfv poc_fft
debug:
	g++ -g -O0 *.cpp -lfftw3 -lsndfile -lm -lpng
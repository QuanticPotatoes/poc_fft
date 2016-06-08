poc_fft:
	g++ *.cpp -lfftw3 -lsndfile -lm -lpng

clean:
	rm -rfv poc_fft
debug:
	g++ -g -O0 *.cpp -lfftw3 -lsndfile -lm -lpng
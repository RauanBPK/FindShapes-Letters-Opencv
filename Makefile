CC=g++
CFLAGS=`pkg-config --cflags --libs opencv`
DEPS = 
OBJ = main.o 

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm main.o 

clean:
	rm main
DIRS=build

CC=g++
CFLAGS=-static-libgcc -static-libstdc++ -Wall

LIBS=-lpng -lz

png2c: main.cpp
	$(shell mkdir -p $(DIRS))
	$(CC) $(CFLAGS) -o "build/png2c" main.cpp $(LIBS)
	
clean:
	rm -f *.o png2c.exe


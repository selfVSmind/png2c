DIRS=build

CC=g++
CFLAGS=-static-libgcc -static-libstdc++ -Wall

LIBS=-lpng -lz

png2c: main.o LibPngHelper.o PixelFormatter.o
	$(shell mkdir -p $(DIRS))
	$(CC) $(CFLAGS) -o "build/png2c" main.o LibPngHelper.o PixelFormatter.o $(LIBS)
	
clean:
	rm -f *.o build/png2c.exe


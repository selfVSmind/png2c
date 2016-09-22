all: convert2h

convert2h: main.o
	g++ -Wall main.o -o convert2h -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -lpng
	
main.o: main.cpp
	g++ -Wall -c main.cpp
	

.PHONY: clean

binaries = convert2h
clean:
	rm -f $(binaries) *.o
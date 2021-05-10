
INCDIR = include
CFLAGS = -std=c++11

all:
	g++ -I. -I$(INCDIR) $(CFLAGS) -c src/*.cpp src/ddsbase/*.cpp
	mv *.o ./Debug
	rm -f *.o
	ar rvs ../lib/libVerdiMain.a Debug/*.o

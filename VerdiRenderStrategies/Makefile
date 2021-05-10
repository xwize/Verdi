
PRJDIR = ../VerdiMain/include
INCDIR = include
CFLAGS = -std=c++11

all:
	g++ -I. -I$(INCDIR) -I$(PRJDIR) -I../Dependencies/boost $(CFLAGS) -c src/*.cpp
	mv *.o ./Debug
	rm -f *.o
	ar rvs ../lib/libVerdiRenderStrategies.a Debug/*.o

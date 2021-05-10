
PRJDIR = ../VerdiMain/include
STGDIR = ../VerdiRenderStrategies
INCDIR = include
CFLAGS = -std=c++11

all:
	g++ -I. -I$(INCDIR) -I$(PRJDIR) -I$(STGDIR) -I../Dependencies/boost $(CFLAGS) -c src/*.cpp
	mv *.o ./Debug
	rm -f *.o
	ar rvs ../lib/libVerdiWorkspace.a Debug/*.o

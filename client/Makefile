CXX = g++ -std=c++11

CFLAGS = -Wall

LDFLAGS = -lm -lpthread -lIrrlicht -lGL -lX11 -lXxf86vm

INCLUDES = ../

main: *.cc *.hh ../defines.hh
	$(CXX) $(CFLAGS) -I $(INCLUDES) *.cc -o $@ $(LDFLAGS)

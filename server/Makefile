CXX = g++ -std=c++11

CFLAGS = -Wall

LDFLAGS = -lm -lpthread

INCLUDES = ../

main: *.cc *.hh ../defines.hh
	$(CXX) $(CFLAGS) -I $(INCLUDES) *.cc -o $@ $(LDFLAGS)

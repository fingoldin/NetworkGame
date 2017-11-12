CXX = g++ -std=c++11

CFLAGS = -Wall

LDFLAGS = -lm -lpthread -lIrrlicht -lGL -lX11 -lXxf86vm

main: *.cc *.hh
	$(CXX) $(CFLAGS) *.cc -o $@ $(LDFLAGS)

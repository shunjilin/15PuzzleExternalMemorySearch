CXX=g++
CXXFLAGS=-Wall -Werror -std=c++11 -DNDEBUG -O3 

all: tiles

tiles:  *.cc */*.cc 
	$(CXX) $(CXXFLAGS) */*.cc *.cc -o tiles

clean:
	rm -fr *.o tiles tiles_solver

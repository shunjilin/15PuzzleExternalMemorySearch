CXX=g++
CXXFLAGS=-Wall -Werror -std=c++98 -DNDEBUG -O3

all: tiles

tiles: *.cc *.hpp
	$(CXX) $(CXXFLAGS) *.cc -o tiles

clean:
	rm -fr *.o tiles tiles_solver

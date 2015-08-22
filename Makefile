all: mst

mst: main.cpp rsgc.hpp mst.hpp bst.hpp value_type.hpp
	$(CXX) -std=c++11 $< -o $@

clean:
	rm mst


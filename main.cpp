#include <vector>
#include "rsgc.hpp"
#include "mst.hpp"


class Coor
{
private:
	int x, y;

public:
	inline void setX(int a) { x = a; }
	inline void setY(int b) { y = b; }
	inline void set(int a, int b) { setX(a); setY(b); }
	inline int getX() const { return x; }
	inline int getY() const { return y; }
};


bool operator== (const Coor& lhs, const Coor& rhs)
{
	return ((lhs.getX() == rhs.getX()) && (lhs.getY() == rhs.getY()));
}


int main()
{
#define PC		20
#define RANGE	1000
#define REPEAT	1000
	for (int rep = 0; rep < REPEAT; ++rep) {
		// random generated point
		Coor coor[PC];
		for (int i = 0; i < PC; ++i) {
			coor[i].set(rand() % RANGE, rand() % RANGE);

			// check there is no two point with the same coordinate
			for (int j = 0; j < i; j++) {
				// re-generate a new point
				if (coor[i] == coor[j]) { --i; break; }
			}
		}

		// generate spanning graph
		std::vector< EDGE<int> > edge_set;
		buildRSG(coor, coor + PC, edge_set);
		// find MST
		bool* mst_edge = new bool[edge_set.size()];
		const int mst_weight1 = findMST(edge_set.begin(), edge_set.end(), mst_edge);
		delete[] mst_edge;
	}
	return 0;
}


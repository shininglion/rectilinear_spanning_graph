/*
 * ----- Find Minimum Spanning Tree (Kruskal) -----
 * This fils provides the implementation of Kruskal method.
 *
 *     ************************************************************************ 
 *     * Copyright (C) 2015 lionking, National Chiao Tung University, Taiwan. * 
 *     * Permission to use, copy, modify, and distribute this                 * 
 *     * software and its documentation for any purpose and without           * 
 *     * fee is hereby granted, provided that the above copyright             * 
 *     * notice appear in all copies.                                         * 
 *     ************************************************************************
 *
 */

#ifndef MST_HPP
#define MST_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <iterator>
#include "value_type.hpp"
#define USE_RVALUE

/************** disjoint set ****************
* table: table[x] is root of x				*
* rank:  rank[x] is depth of x				*
********************************************/
class DisjointSet
{
private:
	int count;
	int *table, *rank;

public:
	DisjointSet() : count(0), table(nullptr), rank(nullptr) {}

	DisjointSet(const int size) : count(size), table(nullptr), rank(nullptr) { makeSet(size); }

	DisjointSet(const DisjointSet& rhs) : count(rhs.count), table(nullptr), rank(nullptr)
	{
		this->table = new int[count];
		this->rank = new int[count];

		memcpy(table, rhs.table, sizeof(int)*count);
		memcpy(rank, rhs.rank, sizeof(int)*count);
	}

	void swap(DisjointSet& rhs)
	{
		std::swap(count, rhs.count);
		std::swap(table, rhs.table);
		std::swap(rank, rhs.rank);
	}

	DisjointSet& operator= (const DisjointSet& rhs)
	{
		DisjointSet temp(rhs);
		swap(temp);
		return *this;
	}

	inline void clear()
	{
		this->count = 0;
		if (this->table) { delete[] this->table; this->table = nullptr; }
		if (this->rank) { delete[] this->rank;  this->rank = nullptr; }
	}

	// initialize this structure
	// parameter:
	// 1. size: total member size
	void makeSet(const int size)
	{
		this->clear();
		this->count = size;
		this->table = new int[size];
		this->rank = new int[size];

		for (int i = 0; i<size; ++i) {
			table[i] = i;
			rank[i] = 0;
		}
	}

	// check the set root of member x belonged to
	// parameter:
	// 1. x: given member
	// return value: set root
	int checkRoot(const int x)
	{
		int root = table[x];
		if (root != x) { return (table[x] = checkRoot(root)); }
		else { return x; }
	}

	// union the sets of members x and y belonged to
	// parameter:
	// 1. x: given member x
	// 2. y: given member y
	void unionSet(const int x, const int y)
	{
		const int root_x = this->checkRoot(x), root_y = this->checkRoot(y);
		if (root_x != root_y) {
			if (rank[root_x] < rank[root_y]) {
				table[root_x] = root_y;
				++rank[root_x];
			}
			else {
				table[root_y] = root_x;
				++rank[root_y];
			}
		}
	}

	~DisjointSet()		{ this->clear(); }

#ifdef USE_RVALUE
	DisjointSet(DisjointSet&& rhs) : count(0), table(nullptr), rank(nullptr) { swap(rhs); }

	DisjointSet& operator= (DisjointSet&& rhs) { swap(rhs); return *this; }
#endif
};

inline void swap(DisjointSet& lhs, DisjointSet& rhs) { lhs.swap(rhs); }


/** graph structure for minimum spanning tree **/
template <typename T>
struct EDGE
{
	int p1, p2;
	T weight;
	typedef T value_type;

	EDGE(int a = 0, int b = 0, T c = T()) : p1(a), p2(b), weight(c) {}
};


/************************** Minimum Spanning Tree (MST) *************************
* find the cost of minimun spanning tree										*
* using Kruskal to solve														*
* usage: r.f. to main															*
* parameter:																	*
* 1. edge: edge[i].weight is the weight of 										*
*		   edge[i].p1 and edge[i].p2											*
* 2. edge_count: amount of edge in this graph									*
* 3. mst_edge: mst_edge[i] is set to true when edge[i] is picked as MST edge	*
* return value: minimum cost													*
*********************************************************************************/
template <typename Iterator>
auto findMST(Iterator edge_begin, Iterator edge_end, bool* mst_edge) -> typename ValueType<typename std::iterator_traits<Iterator>::value_type>::type
{
	typedef typename Iterator::value_type	EDGE_T;
	typedef typename Iterator::reference	EDGE_T_REF;
	typedef typename ValueType<EDGE_T>::type	weight_type;

	weight_type mst_weight = 0;
	const typename Iterator::difference_type edge_count = edge_end - edge_begin;
	DisjointSet ds(edge_count);

	std::fill(mst_edge, mst_edge + edge_count, false);
	std::sort(edge_begin, edge_end, [](const EDGE_T_REF lhs, const EDGE_T_REF rhs)->bool { return lhs.weight < rhs.weight; });

	int index = 0;
	for (auto iter = edge_begin; iter != edge_end; ++iter, ++index) {
		if (ds.checkRoot(iter->p1) != ds.checkRoot(iter->p2)) {
			// find new pair, then union in the same set
			ds.unionSet(iter->p1, iter->p2);
			mst_weight += iter->weight;
			mst_edge[index] = true;
		}
	}

	return mst_weight;
}


#endif


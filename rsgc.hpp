/*
 * ----- Rectilinear Spanning Graph Construction -----
 * Given a set of point, this file gives a set of functions which can generate
 * a rectilinear spanning graph with time complexity O(n log n).
 * The details can reference this blogger:
 * http://shininglionking.blogspot.tw/2014/08/c-efficient-minimum-spanning-tree.html
 *
 * Definitions of the update functions for a given tile and a tile plane.
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

#ifndef RSGC_HPP
#define RSGC_HPP

#include <cstdlib>
#include <cfloat>
#include <vector>
#include <algorithm>
#include <iterator>
#include "mst.hpp"
#include "bst.hpp"


/* Example of MST */
template <typename T>
inline T computeMD(const T x1, const T y1, const T x2, const T y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}


/** build complete graph **/
template <typename Iterator, typename T>
inline void buildCompleteGraph(Iterator first, Iterator last, std::vector< EDGE<T> >& edge_set)
{
	typedef typename std::iterator_traits<Iterator>::difference_type	diff_type;
	typedef typename std::iterator_traits<Iterator>::reference			ref_type;
	const diff_type size = last - first;

	edge_set.reserve(size*(size - 1) / 2);
	int i, j;
	auto beg = first;
	for (i=0; beg != last; ++beg, ++i) {
		auto end = beg;
		for (++end, j=i+1; end != last; ++end, ++j) {
			ref_type p1 = *beg;
			ref_type p2 = *end;
			const int w = computeMD(p1.getX(), p1.getY(), p2.getX(), p2.getY());
			edge_set.emplace_back(i, j, w);
		}
	}
}


template <typename T>
class XLess
{
private:
	T& first;

public:
	XLess(T& f) : first(f) {}
	XLess(const XLess& rhs) : first(rhs.first) {}

	bool operator() (const int lhs, const int rhs)
	{
		typedef typename std::iterator_traits<T>::reference		ref_type;
		ref_type a = first[lhs];
		ref_type b = first[rhs];
		if (a.getX() != b.getX()) { return a.getX() < b.getX(); }
		else { return a.getY() < b.getY(); }
	}
};


template <typename T>
class YLess
{
private:
	T& first;

public:
	YLess(T& f) : first(f) {}
	YLess(const YLess& rhs) : first(rhs.first) {}

	bool operator() (const int lhs, const int rhs)
	{
		typedef typename std::iterator_traits<T>::reference		ref_type;
		ref_type a = first[lhs];
		ref_type b = first[rhs];
		if (a.getY() != b.getY()) { return a.getY() < b.getY(); }
		else { return a.getX() < b.getX(); }
	}
};


template <typename T>
class YLarge
{
private:
	T& first;

public:
	YLarge(T& f) : first(f) {}
	YLarge(const YLarge& rhs) : first(rhs.first) {}

	bool operator() (const int lhs, const int rhs)
	{
		typedef typename std::iterator_traits<T>::reference		ref_type;
		ref_type a = first[lhs];
		ref_type b = first[rhs];
		if (a.getY() != b.getY()) { return a.getY() > b.getY(); }
		else { return a.getX() < b.getX(); }
	}
};


/********************** build rectilinear spanning graph(RSG) ***********************
 * Reference:																		*
 * Hai Zhou, Narendra Shenoy and William Nicholls,									*
 * "Efficient Minimum Spanning Tree Construction without Delaunay Triangulation",	*
 * ASP-DAC, 2001																	*
 ************************************************************************************/
template <typename RandomAccessIterator, typename T>
void buildRSG(RandomAccessIterator first, RandomAccessIterator last, std::vector< EDGE<T> >& edge_set)
{
	typedef typename std::iterator_traits<RandomAccessIterator>::value_type			val_type;
	typedef typename std::iterator_traits<RandomAccessIterator>::reference			ref_type;
	typedef typename std::iterator_traits<RandomAccessIterator>::difference_type	diff_type;
	const diff_type size = last - first;

	// too few points, construct complete graph directly
	if (size <= 25) {
		buildCompleteGraph(first, last, edge_set);
	}
	// build rectilinear spanning graph according to following paper:
	// 
	else {
		int* index = new int[size];
		for (int i = 0; i < size; ++i) { index[i] = i; }

		XLess<RandomAccessIterator> xless = XLess<RandomAccessIterator>(first);
		YLess<RandomAccessIterator> yless = YLess<RandomAccessIterator>(first);
		BST< int, XLess<RandomAccessIterator> > as1(xless);	// R1 active set
		BST< int, YLess<RandomAccessIterator> > as2(yless);	// R2 active set

		// poins are sorted with respect to x + y
		std::sort(index, index+size, 
			[&](const int lhs, const int rhs) -> bool { 
				ref_type a = first[lhs];
				ref_type b = first[rhs];
				return (a.getX() + a.getY()) < (b.getX() + b.getY());
			}
		);
		for (int i = 0; i < size; ++i) {
			ref_type a = first[ index[i] ];
			// find nearest point which has a in its R1 region
			T min_md = T();
			int min_index = -1;
			for (int* pt = as1.queryMaxL(index[i]); pt != nullptr; pt = as1.queryMaxL(*pt)) {
				ref_type b = first[*pt];
				if ((b.getX() - b.getY()) <= (a.getX() - a.getY())) { break; }

				// check if a is inside the R1 region of b
				double slope = DBL_MAX;
				if (a.getX() != b.getX()) { slope = static_cast<double>(b.getY() - a.getY()) / static_cast<double>(b.getX() - a.getX()); }
				if (slope > 1) {
					const T wl = computeMD(a.getX(), a.getY(), b.getX(), b.getY());
					if (min_index < 0 || wl < min_md) { min_index = *pt; min_md = wl; }
				}
			}
			// add new edge
			if (min_index >= 0) {
				edge_set.emplace_back(index[i], min_index, min_md);
				// remove from active set
				as1.erase(min_index);
			}
			// push into active set
			as1.insert(index[i]);

			// find nearest point which has a in its R2 region
			min_index = -1; min_md = T();
			for (int* pt = as2.queryMaxL(index[i]); pt != nullptr; pt = as2.queryMaxL(*pt)) {
				ref_type b = first[*pt];
				if ((b.getX() - b.getY()) > (a.getX() - a.getY())) { break; }

				// check if a is inside the R2 region of b
				double slope = DBL_MAX;
				if (a.getX() != b.getX()) { slope = static_cast<double>(b.getY() - a.getY()) / static_cast<double>(b.getX() - a.getX()); }
				if ((slope > 0) && (slope <= 1)) {
					const T wl = computeMD(a.getX(), a.getY(), b.getX(), b.getY());
					if (min_index < 0 || wl < min_md) { min_index = *pt; min_md = wl; }
				}
			}
			// add new edge
			if (min_index >= 0) {
				edge_set.emplace_back(index[i], min_index, min_md);
				// remove from active set
				as2.erase(min_index);
			}
			// push into active set
			as2.insert(index[i]);
		}
		as1.clear(); as2.clear();

		YLarge<RandomAccessIterator> ylarge = YLarge<RandomAccessIterator>(first);
		BST< int, YLarge<RandomAccessIterator> > as3(ylarge);	// R3 active set
		BST< int, XLess<RandomAccessIterator> > as4(xless);	// R4 active set

		// poins are sorted with respect to x - y
		std::sort(index, index + size,
			[&](const int lhs, const int rhs) -> bool {
				ref_type a = first[lhs];
				ref_type b = first[rhs];
				return (a.getX() - a.getY()) < (b.getX() - b.getY());
			}
		);
		for (int i = 0; i < size; ++i) {
			ref_type a = first[index[i]];

			// find nearest point which has a in its R3 region
			T min_md = T();
			int min_index = -1;
			for (int* pt = as3.queryMaxL(index[i]); pt != nullptr; pt = as3.queryMaxL(*pt)) {
				ref_type b = first[*pt];
				if ((b.getX() + b.getY()) >= (a.getX() + a.getY())) { break; }

				// check if a is inside the R3 region of b
				double slope = DBL_MIN;
				if (a.getX() != b.getX()) { slope = static_cast<double>(b.getY() - a.getY()) / static_cast<double>(b.getX() - a.getX()); }
				if ((slope <= 0) && (slope > -1)) {
					const T wl = computeMD(a.getX(), a.getY(), b.getX(), b.getY());
					if (min_index < 0 || wl < min_md) { min_index = *pt; min_md = wl; }
				}
			}
			// add new edge
			if (min_index >= 0) {
				edge_set.emplace_back(index[i], min_index, min_md);
				// remove from active set
				as3.erase(min_index);
			}
			// push into active set
			as3.insert(index[i]);

			// find nearest point which has a in its R4 region
			min_index = -1; min_md = T();
			for (int* pt = as4.queryMaxL(index[i]); pt != nullptr; pt = as4.queryMaxL(*pt)) {
				ref_type b = first[*pt];
				if ((b.getX() + b.getY()) < (a.getX() + a.getY())) { break; }

				// check if a is inside the R4 region of b
				double slope = DBL_MIN;
				if (a.getX() != b.getX()) { slope = static_cast<double>(b.getY() - a.getY()) / static_cast<double>(b.getX() - a.getX()); }
				if (slope <= -1) {
					const T wl = computeMD(a.getX(), a.getY(), b.getX(), b.getY());
					if (min_index < 0 || wl < min_md) { min_index = *pt; min_md = wl; }
				}
			}
			// add new edge
			if (min_index >= 0) {
				edge_set.emplace_back(index[i], min_index, min_md);
				// remove from active set
				as4.erase(min_index);
			}
			// push into active set
			as4.insert(index[i]);
		}

		delete[] index;
	}
}


#endif

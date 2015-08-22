/*
 * This fils provides the implementation of a binary search tree.
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
#ifndef BST_HPP
#define BST_HPP

#include <queue>
#include <algorithm>
#include <functional>


template <typename T>
struct BSTNode
{
	typedef T value_type;

	value_type data;	// data stored in this node
	BSTNode<T>* lc;	// left child
	BSTNode<T>* rc;	// right child

	BSTNode<T>() : data(T()), lc(nullptr), rc(nullptr) {}
};


/******** Binary Search Tree ********
 * Template parameter:				*
 * 1. T: type of stored data		*
 * 2. Compare: compared object type	*
 ************************************/
template < typename T, typename Compare = std::less<T> >
class BST
{
private:
	BSTNode<T>*	root;
	Compare		cmp;

	void clearTree(BSTNode<T>* ptr);
	void insertToTree(const T& data, BSTNode<T>** pptr);
	void eraseFromTree(const T& data, BSTNode<T>** pptr);
	T* queryTreeEqual(const T& data, BSTNode<T>* ptr);
	T* queryTreeMaxL(const T& data, BSTNode<T>* ptr);
	T* queryTreeMinG(const T& data, BSTNode<T>* ptr);

public:
	typedef T value_type;

	BST(const Compare& compare = Compare()) : root(nullptr), cmp(compare) {}
	BST(const BST<T, Compare>& rhs);
	BST(BST<T, Compare>&& rhs) : root(nullptr), cmp(rhs.cmp) { swap(rhs); }
	~BST() { clear(); }
	BST<T, Compare>& operator= (const BST<T, Compare>& rhs)
	{
		BST<T> temp(rhs);
		swap(temp, root);
		return *this;
	}
	BST<T, Compare>& operator= (BST<T, Compare>&& rhs)	{ swap(rhs); return *this; }

	// swap content with another bst
	inline void swap(BST<T, Compare>& rhs)	{ std::swap(root, rhs, root); }
	
	// remove all of the data
	inline void clear()	{ clearTree(root); root = nullptr; }
	
	// insert a data into bst
	inline void insert(const T& data)	{ insertToTree(data, &root); }
	
	// remove requested data from bst
	inline void erase(const T& data)	{ eraseFromTree(data, &root); }

	// return a pointer points to the data which is equal to the query data
	// return nullptr when the query data is not in this tree
	inline T* queryEqual(const T& data)	{ return queryTreeEqual(data, root); }

	// return a pointer points to the maximum data which is less than the query data
	// return nullptr when there is no such data in this tree
	inline T* queryMaxL(const T& data)	{ return queryTreeMaxL(data, root); }

	// return a pointer points to the minimum data which is greater than the query data
	// return nullptr when there is no such data in this tree
	inline T* queryMinG(const T& data)	{ return queryTreeMinG(data, root); }
};


template <typename T, typename Compare>
BST<T, Compare>::BST(const BST<T, Compare>& rhs) : root(nullptr), cmp(rhs.cmp)
{
	if (rhs.root == nullptr) { return; }
	root = new BSTNode<T>();

	// lhstree: this tree
	std::queue<BSTNode<T>*> lhstree;
	// tree stored in rhs
	std::queue<BSTNode<T>*> rhstree;
	// push root into trees
	lhstree.push(root);
	rhstree.push(rhs.root);

	while (lhstree.empty() == false) {
		BSTNode<T>* lnode = lhstree.front(); lhstree.pop();
		BSTNode<T>* rnode = rhstree.front(); rhstree.pop();

		// copy data
		lnode->data = rnode->data;
		// left child is not empty
		if (rnode->lc != nullptr) {
			lnode->lc = new BSTNode<T>();
			lhstree.push(lnode->lc);
			rhstree.push(rnode->lc);
		}
		// right child is not empty
		if (rnode->rc != nullptr) {
			lnode->rc = new BSTNode<T>();
			lhstree.push(lnode->rc);
			rhstree.push(rnode->rc);
		}
	}
}


template <typename T, typename Compare>
void BST<T, Compare>::clearTree(BSTNode<T>* ptr)
{
	if (ptr == nullptr) { return; }
	if (ptr->lc != nullptr) { clearTree(ptr->lc); }
	if (ptr->rc != nullptr) { clearTree(ptr->rc); }
	delete ptr;
}


template <typename T, typename Compare>
T* BST<T, Compare>::queryTreeEqual(const T& data, BSTNode<T>* ptr)
{
	// ptr is nullptr
	if (ptr == nullptr) { return nullptr; }

	// data is less than ptr->data
	const bool lt = cmp(data, ptr->data);
	// data is greater than ptr->data
	const bool gt = cmp(ptr->data, data);

	// when both lt and gt are false, data is equal to ptr->data
	if ((lt==false) && (gt==false)) { return &(ptr->data); }

	// data is less than ptr->data, go left
	if (lt) { return queryTreeEqual(data, ptr->lc); }
	// data is greater than ptr->data, go right
	if (gt) { return queryTreeEqual(data, ptr->rc); }
}


template <typename T, typename Compare>
T* BST<T, Compare>::queryTreeMaxL(const T& data, BSTNode<T>* ptr)
{
	// ptr is nullptr
	if (ptr == nullptr) { return nullptr; }

	// data is greater than ptr->data
	const bool gt = cmp(ptr->data, data);

	// data is greater than ptr->data, go right
	if (gt) {
		T* result = queryTreeMaxL(data, ptr->rc);
		if (result == nullptr) { return &(ptr->data); }
		else if (cmp(*result, ptr->data)) { return &(ptr->data); }
		else { return result; }
	}
	// data is less than or equal to ptr->data, go left
	else { return queryTreeMaxL(data, ptr->lc); }
}


template <typename T, typename Compare>
T* BST<T, Compare>::queryTreeMinG(const T& data, BSTNode<T>* ptr)
{
	// ptr is nullptr
	if (ptr == nullptr) { return nullptr; }

	// data is less than ptr->data
	const bool lt = cmp(data, ptr->data);

	// data is less than or equal to ptr->data, go right
	if (lt) {
		T* result = queryTreeMinG(data, ptr->lc);
		if (result == nullptr) { return &(ptr->data); }
		if (cmp(ptr->data, *result)) { return &(ptr->data); }
		else { return result; }
	}
	// data is greater than ptr->data, go left
	else { return queryTreeMinG(data, ptr->rc); }
}


template <typename T, typename Compare>
void BST<T, Compare>::insertToTree(const T& data, BSTNode<T>** pptr)
{
	BSTNode<T>* (&ptr) = *pptr;

	// create node
	if (ptr == nullptr) {
		ptr = new BSTNode<T>();
		ptr->data = data;
		return;
	}

	// data is less than ptr->data
	const bool lt = cmp(data, ptr->data);
	// data is greater than ptr->data
	const bool gt = cmp(ptr->data, data);

	// when both lt and gt are false, data is equal to ptr->data
	// in this case, do nothing
	if ((lt == false) && (gt == false)) { return; }

	// data is less than ptr->data, go left
	if (lt) { return insertToTree(data, &(ptr->lc)); }
	// data is greater than ptr->data, go right
	if (gt) { return insertToTree(data, &(ptr->rc)); }
}


template <typename T, typename Compare>
void BST<T, Compare>::eraseFromTree(const T& data, BSTNode<T>** pptr)
{
	BSTNode<T>* (&ptr) = *pptr;

	// do nothing
	if (ptr == nullptr) { return; }

	// data is less than ptr->data
	const bool lt = cmp(data, ptr->data);
	// data is greater than ptr->data
	const bool gt = cmp(ptr->data, data);

	// when both lt and gt are false, data is equal to ptr->data
	// then remove this data
	if ((lt == false) && (gt == false)) {
		BSTNode<T>* temp = nullptr;
		// case 1: a leaf node, delete this node directly
		if ((ptr->lc == nullptr) && (ptr->rc == nullptr)) { delete ptr; ptr = nullptr; }
		// case 2: this node has no left child, then move pointer to point to its right child directly
		else if (ptr->lc == nullptr) { temp = ptr; ptr = ptr->rc; delete temp; }
		// case 3: this node has no right child, then move pointer to point to its left child directly
		else if (ptr->rc == nullptr) { temp = ptr; ptr = ptr->lc; delete temp; }
		// case 4: the largest value in the left sub-tree is the left child itself
		//         make the right child of the left child to point to 
		else if (ptr->lc->rc == nullptr) { temp = ptr; ptr->lc->rc = ptr->rc; ptr = ptr->lc; delete temp; }
		// case 5: general case, find the largest data at the left sub-tree
		else {
			BSTNode<T>* head = ptr->lc;
			// move to the right-most node of left sub-tree (denoted: rmleaf)
			// note: head is the parent node of rmleaf
			for (; head->rc->rc != nullptr; head = head->rc);
			// replace current data with the data at rmleaf
			ptr->data = head->rc->data;
			// if the left child of rmleaf is not empty, the same to case 3
			if (head->rc->lc != nullptr) { temp = head->rc; head->rc = head->rc->lc; delete temp; }
			// otherwise, rmleaf is a leaf node
			else { delete head->rc; head->rc = nullptr; }
		}
		return;
	}

	// data is less than ptr->data, go left
	if (lt) { return eraseFromTree(data, &(ptr->lc)); }
	// data is greater than ptr->data, go right
	if (gt) { return eraseFromTree(data, &(ptr->rc)); }
}

#endif

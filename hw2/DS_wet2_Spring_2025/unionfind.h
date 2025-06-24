#ifndef UNIONFIND_SLOW_H
#define UNIONFIND_SLOW_H

#include <assert.h>
#include <stdio.h>
#include <iostream>
// #include "hashtable.h"
// #include "hashtable_doublehashing.h"
#include "hashtable_chainhashing.h"

template<class T>
class Node {
public:
    // type T must implement operator< to determine the id of the parent
    // when doing union
    T value;
    int size;
    int baseRank;
    int relRank;
    Node *parent;

    Node(const T& value)
	: value(value),size(1),baseRank(0),relRank(0),parent(this) {}
    ~Node() = default;

    // return true if this node doesn't have a parent
    bool isHead() const {
	assert(parent != nullptr);
	if(parent==this) {
	    assert(relRank==0);
	    return true;
	} else {
	    return false;
	}
    }
};

// assumed value is a struct with field id
template<class T>
int defaultKeyFn(const T& value) {
    return value.id;
}
template<class T>
class UnionFind
{
public:
    HashTable<int,Node<T>*> elements;
    int (*keyfn)(const T&);
    //! Default constructor
    UnionFind(int (*keyf)(const T&)) : elements(),keyfn(keyf)
    {}
    UnionFind() : UnionFind(defaultKeyFn)
    {}
    //! Copy constructor
    UnionFind(const UnionFind &other) = default;

    //! Move constructor
    UnionFind(UnionFind &&other) noexcept = default;

    //! Destructor
    virtual ~UnionFind() noexcept;

    //! Copy assignment operator
    UnionFind& operator=(const UnionFind &other);

    //! Move assignment operator
    UnionFind& operator=(UnionFind &&other) noexcept;

    // given a value, will try to create a new set containing only this value.
    // return true if the operation is successful, otherwise value is already
    // in a set and returns false.
    bool makeSet(const T& value);
    // return the representative of the set that contains value which is associated with the id. If not found, return nullptr
    Node<T>* find(const int id);
    // given two ids, will union the sets that correspond to each value associated with the id. Returns true if there was a union, false otherwise
    bool unionSets(const int id1,const int id2);
    int getAbsoluteRank(int gen) ;
    // used to speed implementation. Will return the top most parent of element ele
    // and update the parent of all nodes along the path
    Node<T>* find_root(Node<T>* ele);

    int rank(int fleetId);
protected:
private:
};
template<class T>
int UnionFind<T>::rank(int fleetId) {
    Node<T>* flt = elements.find(fleetId);
    int rank = 0;
    while(flt != flt->parent) {
	rank += flt->relRank;
	flt = flt->parent;
    }
    rank += flt->baseRank;
    return rank;
    // Node<T>* flt = find(fleetId);
    // if(flt==nullptr) {
    // 	return -1;
    // }
    // assert(flt->parent->isHead());
    // return flt->relRank + flt->parent->baseRank;

}
// Helper: Get the absolute "rank" (genre changes so far) from this node to the root.
template<class T>
int UnionFind<T>::getAbsoluteRank(int fleetId) {
    Node<T>* flt = elements.find(fleetId);
    int sum = 0;
    // walk up to root, accumulating relRank
    while (flt != flt->parent) {
        sum += flt->relRank;
        flt = flt->parent;
    }
    // finally add root’s baseRank
    sum += flt->baseRank;
    return sum;
}

template<class T>
bool UnionFind<T>::makeSet(const T& value) {
    int id = keyfn(value);
    if(elements.contains(id)) {
	return false;
    }
    Node<T> *n = new Node<T>(value);
    if(n == nullptr) {
	return false;
    }
    
    elements.insert(id,n);
    return true;
}
template<class T>
Node<T>* UnionFind<T>::find(const int id) {
    if(!elements.contains(id)) {
	return nullptr;
    }
    Node<T>* ele = elements.find(id);
    assert(ele != nullptr);
    return find_root(ele);
}
template<class T>
Node<T>* UnionFind<T>::find_root(Node<T>* ele) {
    assert(ele!=nullptr);
    if(ele != ele->parent) {
	Node<T>* org_parent = ele->parent;
	ele->parent = find_root(org_parent);
	ele->relRank += org_parent->relRank;
    } else {
	assert(ele->relRank == 0);
    }
    return ele->parent;
}
template<class T>
bool UnionFind<T>::unionSets(const int id1,const int id2) {
    Node<T>* big_set = nullptr;
    Node<T>* small_set = nullptr;
    {
	Node<T>* rep1 = find(id1);
	Node<T>* rep2 = find(id2);
	assert(rep1 != nullptr && rep2 != nullptr);
	assert(rep1->isHead() && rep2->isHead());
	big_set = rep1;
	small_set = rep2;
	if(rep1->size < rep2->size) {
	    big_set = rep2;
	    small_set = rep1;
	}
    }
    if(big_set == small_set)
	return false;

    small_set->parent = big_set;
    big_set->size += small_set->size;
    small_set->size = 0;
    return true;
}

template<class T>
UnionFind<T>::~UnionFind() noexcept {
    auto end = elements.end();
    for(auto it = elements.begin(); it != end; ++it) {
	delete (*it).second;
    }
}
#endif /* UNIONFIND_SLOW_H */

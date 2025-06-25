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
    int  Modefied_Union(int gen1, int gen2, int gen3 ,shared_ptr< HashTable<int,shared_ptr< Genre>>> Genres); 
    int Modefied_find(int songid, shared_ptr< HashTable<int,shared_ptr<Song>>> songs) ; 
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
   int UnionFind<T>::Modefied_find(int songid, shared_ptr< HashTable<int,shared_ptr<Song>>> songs) {
        auto songNode = songs->find(songid);
        if(songNode == nullptr) {
            return 0;
        }
        int sum = 0;
        auto temp1 = songNode ; 
        while(temp1->parent != nullptr) {
            sum += temp1->merges;
            temp1 = temp1->parent;
        }

        int sub=0;
         shared_ptr<Song> temp2 = songNode;

        while(temp2->parent != nullptr && temp2->parent != temp1){
            auto temp= temp2;
            int orgin = temp2->merges;
            temp2->merges = sum - sub;
            sub += orgin;
            temp2 = temp2->parent;
            temp->parent = temp1;
        }

        if(temp1->genre_root == nullptr) return 0;
        return temp1->genre_root->id;
    }

template<class T>
 int UnionFind<T>::Modefied_Union(int gen1, int gen2, int gen3 ,shared_ptr< HashTable<int,shared_ptr< Genre>>> Genres) {
        auto g1 = Genres->find(gen1);
        auto g2 = Genres->find(gen2);
        auto g3 = Genres->find(gen3);
        if(g3) return 0;
        if(g1 == nullptr || g2 == nullptr || g1 == g2 ){
            return 0;
        }
        
        shared_ptr<Genre> newgen =  make_shared<Genre>(gen3);
        Genres->insert(gen3,newgen);
        auto song1 = g1->root_in_songs.lock();
        auto song2 = g2->root_in_songs.lock();
        // here we set  and upadte the size ,  whos the parent and whos the root in the union.
        if(song1 == nullptr && song2 == nullptr){return 1; }

        if(song1 == nullptr || song2 == nullptr){
            
          if (song1 == nullptr){
                newgen->songCount = g2->songCount;
                newgen->root_in_songs = song2;
                song2->genre_root = newgen;
                song2->merges++;
                g1 ->songCount = 0;
                g2 ->songCount = 0;
                g1 ->root_in_songs.reset() ; 
                g2 ->root_in_songs.reset() ; 
                return 1;
            }
            else {
                newgen ->songCount = g1 ->songCount;
                newgen ->root_in_songs = song1;
                song1->genre_root = newgen ;
                song1->merges++;
                g1 ->songCount = 0;
                g2 ->songCount = 0;
                g1 ->root_in_songs.reset(); 
                g2 ->root_in_songs.reset();
                return 1;
          
            }
        }
        
        // here we union 
        if(g1->songCount >= g2->songCount){
            song2->parent = song1;
            song2->merges -= song1->merges;
            song1->genre_root->songCount += song2->genre_root->songCount;
            song1->merges++;
            newgen ->root_in_songs = song1;
            newgen ->songCount = g1 ->songCount;
            song1->genre_root = newgen ;
            song2->genre_root = nullptr;
        }

        else {
            song1->parent = song2;
            song1->merges -= song2->merges;
            song2->genre_root->songCount += song1->genre_root->songCount;
            song2->merges++;
            newgen ->root_in_songs = song2;
            newgen ->songCount = g2 ->songCount;
            song2->genre_root = newgen ;
            song1->genre_root = nullptr;
        }
        // extra for safe 
        g1 ->songCount = 0;
        g2 ->songCount = 0;
        g1 ->root_in_songs.reset();
        g2 ->root_in_songs.reset();

        return 1;

    }

template<class T>
UnionFind<T>::~UnionFind() noexcept {
    }
#endif /* UNIONFIND_SLOW_H */

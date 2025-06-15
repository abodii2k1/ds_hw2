#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <assert.h>
#include <math.h>


namespace hashtable{
    template<class K,class V>
    struct pair{
	K first;
	V second;
	pair(K& f,V& s) : first(f),second(s)
	{}
    };
    template<class K,class V>
    struct Node {
	K key;
	V value;
	Node<K,V>* next;

    };
    template<class K,class V>
    void deleteList(Node<K,V>* node) {
	Node<K,V>* it=node;
	while(it != nullptr) {
	    Node<K,V>* next = it->next;
	    delete it;
	    it = next;
	}
    }
    template<class K,class V>
    Node<K,V>* copyList(Node<K,V>* head) {
	if(head == nullptr)
	    return nullptr;
	Node<K,V>* copy_head = new Node<K,V>(*head);
	Node<K,V>* it_copy = copy_head;
	Node<K,V>* it = head;
	while(it->next != nullptr) {
	    Node<K,V>* node = new Node<K,V>(*it->next);
	    it_copy->next = node;
	
	    it_copy = it_copy->next;
	    it = it->next;
	}
	return copy_head;
    }
}
template<class K,class V>
class HashTable
{
public:
    // NOTE(Salim): should we replace int with size_t? it could convey the meaning better.
    const static int min_capacity = 5;
    int len;
    int capacity;
    hashtable::Node<K,V>* table;
    int (*key2int)(const K&);
    //! Default constructor
    HashTable();
    HashTable(int (*key2int_f)(const K&),int s_capacity = 0);
    //! Copy constructor
    HashTable(const HashTable &other);
    
    //! Move constructor
    HashTable(HashTable &&other) noexcept = delete;
    
    //! Destructor
    ~HashTable() noexcept;
    
    //! Copy assignment operator
    HashTable& operator=(const HashTable &other);
    
    //! Move assignment operator
    HashTable& operator=(HashTable &&other) noexcept;
    //delete the value for this key 
    bool deleteValue(const  K key ,const V val) ;  
    // returns true if key has a corresponding value
    bool contains(const K key) const;
    // given a key and value, will insert the key value pair into the table.
    // if key already exists, the map is not modified.
    void insert(const K key,const V& val);
    void insert_record(const K key,const V& val);
    // returns the value corresponding to the key. assumes the key exists
    V& find(const K key);
    // delete value that corresponds to a key. Return true if the key existed
    bool deleteEntry(const K key);
    // resize hashtable if there are too many elements or too few elements relative to the capacity
    // return false if there was allocation problem
     V& findByValue(const K key ,const V val) ; 
     int howManyContains(const K key ) const  ; 
    bool resizeHashTable();
    bool resizeHashTable_record() ; 
    hashtable::Node<K,V>* insertAssumeCapacity(const K key,const V& val,bool *exists);
    hashtable::Node<K,V>* insertAssumeCapacity_record(const K key,const V& val,bool *exists);


    class Iterator;
    Iterator begin();
    Iterator end();

    void print();
    void printToBoth();
protected:
private:
   int hashKey(const K& key) const {
    static long double multiplier = 0.5 * (sqrt(5) - 1);
    int hash = floor(capacity * (multiplier * key2int(key)));
    return (hash % capacity + capacity) % capacity; // Adjust to ensure non-negative hash
}
};

template<class K>
int identity(const K& key) {
    return key;
}
template<class K,class V>
HashTable<K,V>::HashTable() : HashTable(identity)
{}

template<class K,class V>
HashTable<K,V>::HashTable(int (*key2int_f)(const K&),int s_capacity) : len(0),capacity(s_capacity),table(nullptr),key2int(key2int_f)
{
    if(capacity < min_capacity) {
	capacity = min_capacity;
    }
    table = new hashtable::Node<K,V>[capacity];
    for(int i = 0; i<capacity;i++) {
	table[i].next = nullptr;
    }
}

template<class K,class V>
HashTable<K,V>& HashTable<K,V>::operator=(const HashTable &other) {
    if(this==&other) {
	return *this;
    }
    int old_len = len;
    int old_capacity = capacity;
    hashtable::Node<K,V>* old_table = table;
    table = new hashtable::Node<K,V>[other.capacity];
    try {
	len = other.len;
	capacity = other.capacity;
	for(int i = 0; i<other.capacity; i++) {
	    hashtable::Node<K,V>* lst = copyList(other.table[i].next);
	    table[i].next = lst;
	}
	if(old_table != nullptr) {
	    for(int i = 0;i<old_capacity;i++) {
		deleteList(old_table[i].next);
	    }
	    delete[] old_table;
	}
	return *this;
    } catch(...) {
	len = old_len;
	capacity = old_capacity;
	for (int i = 0; i<other.capacity; ++i) {
	    deleteList(table[i].next);
	}
	table = old_table;
	throw;
    }
}
template<class K,class V>
HashTable<K,V>::~HashTable() noexcept {
    for (int i = 0; i<capacity; ++i) {
	    hashtable::Node<K,V>* iter = table[i].next;
	    deleteList(iter);
	}
    delete[] table;
}

template<class K,class V>
bool HashTable<K,V>::contains(const K key) const {
    int pos = hashKey(key);
    assert(pos>=0 && pos<capacity);
    for(hashtable::Node<K,V>* it = table[pos].next; it != nullptr; it = it->next) {
	if(key == it->key) {
	    return true;
	}
    }
    return false;
}

template<class K,class V>
int HashTable<K,V>::howManyContains(const K key ) const {
    int pos = hashKey(key);
    int count = 0 ; 
    assert(pos>=0 && pos<capacity);
    for(hashtable::Node<K,V>* it = table[pos].next; it != nullptr; it = it->next) {
	if(key == it->key) { 
	    count++ ; 
	}
    }
    return count ; 
}

template<class K,class V>
hashtable::Node<K,V>* HashTable<K,V>::insertAssumeCapacity(const K key,const V& val,bool *exists) {
    int pos = hashKey(key);
    assert(pos>=0 && pos<capacity);
    if(contains(key)) {
	if(exists != nullptr) {
	    *exists = true;
	}
	return nullptr;
    }
    hashtable::Node<K,V>* last = &table[pos];
    for(long long __guard = 1000000000000; last->next != nullptr; last = last->next, __guard--) {
	assert(last->next->key != key);
	if(__guard==0) {
#ifndef NDEBUG
	    std::cout << "__guard was breached in insert\n";
	    return nullptr;
#endif
	}
    }
    hashtable::Node<K,V>* n = new hashtable::Node<K,V>();
    if(n==nullptr) {
	if(exists != nullptr) {
	    *exists = true;
	}
	return nullptr;
    }
    n->key = key;
    n->value = val;
    last->next = n;
    len++;
    return n;
} 

template<class K,class V>
void HashTable<K,V>::insert(const K key,const V& val) {
    if(!resizeHashTable()) {
#ifndef NDEBUG
	std::cout << "failed to resize table while doing insert";
	return;
#endif
    }
    // bool exists = false;
    // Node<K,V>* node = insertAssumeCapacity(key,val,&exists);
    (void)insertAssumeCapacity(key,val,nullptr);
}

template<class K,class V>
hashtable::Node<K,V>* HashTable<K,V>::insertAssumeCapacity_record(const K key,const V& val,bool *exists) {
    int pos = hashKey(key);
    assert(pos>=0 && pos<capacity);
    hashtable::Node<K,V>* last = &table[pos];
    for(long long __guard = 1000000000000; last->next != nullptr; last = last->next, __guard--) {
	assert(last->next->key != key);
	if(__guard==0) {
#ifndef NDEBUG
	    std::cout << "__guard was breached in insert\n";
	    return nullptr;
#endif
	}
    }
    hashtable::Node<K,V>* n = new hashtable::Node<K,V>();
    if(n==nullptr) {
	if(exists != nullptr) {
	    *exists = true;
	}
	return nullptr;
    }
    n->key = key;
    n->value = val;
    last->next = n;
    len++;
    return n;
} 

template<class K,class V>//Rawi : added here ?
void HashTable<K,V>::insert_record(const K key,const V& val) {
    if(!resizeHashTable_record()) {
#ifndef NDEBUG
	std::cout << "failed to resize table while doing insert";
	return;
#endif
    }
    // bool exists = false;
    // Node<K,V>* node = insertAssumeCapacity(key,val,&exists);
    (void)insertAssumeCapacity_record(key,val,nullptr);
}
template<class K,class V>
V& HashTable<K,V>::findByValue(const K key , const V val) {
    assert(contains(key) && "key is not found in find function");
    int pos = hashKey(key);
    assert(pos>=0 and pos<capacity);
    for(hashtable::Node<K,V>* it = table[pos].next; it != nullptr; it = it->next) {
	if(key == it->key ) {
        if(val == it->value){
          return it->value;
        }
	    
	}
    }
    assert(false && "reach Undefined state in find");
    return table[pos].value;
}

template<class K,class V>
V& HashTable<K,V>::find(const K key) {
    assert(contains(key) && "key is not found in find function");
    int pos = hashKey(key);
    assert(pos>=0 and pos<capacity);
    for(hashtable::Node<K,V>* it = table[pos].next; it != nullptr; it = it->next) {
	if(key == it->key) {
	    return it->value;
	}
    }
    assert(false && "reach Undefined state in find");
    return table[pos].value;
}
template<class K,class V>
bool HashTable<K,V>::deleteValue(const K key ,const V val) {
  int pos = hashKey(key);
    assert(pos>=0 and pos<capacity);
    int found = false;
    for(hashtable::Node<K,V>* it = &table[pos]; it->next != nullptr; it = it->next) {
	if(key == it->next->key && val == it->next->value) {
	    hashtable::Node<K,V>* entry = it->next;
	    it->next = entry->next;
	    found = true;
	    delete entry;
	    break;
	}
    }
  
    len-- ; 
    return found;
}
template<class K,class V>
bool HashTable<K,V>::deleteEntry(const K key) {
    int pos = hashKey(key);
    assert(pos>=0 and pos<capacity);
    int found = false;
    for(hashtable::Node<K,V>* it = &table[pos]; it->next != nullptr; it = it->next) {
	if(key == it->next->key) {
	    hashtable::Node<K,V>* entry = it->next;
	    it->next = entry->next;
	    found = true;
	    delete entry;
	    break;
	}
    }
    return found;
}
template<class K,class V>
bool HashTable<K,V>::resizeHashTable() {
    int newCap = min_capacity;
    if(capacity == 0 || len > 2*capacity) {
	newCap = capacity > 0 ? 2*capacity : min_capacity;
    } else if(capacity > min_capacity && len * 4 < capacity) {
	newCap = capacity/2;
    } else {
	return true;
    }
    try {
	HashTable<K,V> newTable(key2int,newCap);
	for(int i = 0;i<this->capacity;i++) {
	    for(hashtable::Node<K,V>* it = this->table[i].next;it != nullptr ; it=it->next) {
		bool exists = false;
		(void)newTable.insertAssumeCapacity(it->key,it->value,&exists);
		assert(!exists && "two keys found while doing resize");
	    }
	}
	*this = newTable;
	return true;
    } catch(...) {
	return false;
    }
}

template<class K,class V>
bool HashTable<K,V>::resizeHashTable_record() {
    int newCap = min_capacity;
    if(capacity == 0 || len > 2*capacity) {
	newCap = capacity > 0 ? 2*capacity : min_capacity;
    } else if(capacity > min_capacity && len * 4 < capacity) {
	newCap = capacity/2;
    } else {
	return true;
    }
    try {
	HashTable<K,V> newTable(key2int,newCap);
	for(int i = 0;i<this->capacity;i++) {
	    for(hashtable::Node<K,V>* it = this->table[i].next;it != nullptr ; it=it->next) {
		bool exists = false;
		(void)newTable.insertAssumeCapacity_record(it->key,it->value,&exists);
		assert(!exists && "two keys found while doing resize");
	    }
	}
	*this = newTable;
	return true;
    } catch(...) {
	return false;
    }
}


template<class K,class V>
typename HashTable<K,V>::Iterator HashTable<K,V>::begin() {
    int pos = 0;
    while(table[pos].next == nullptr) {
	pos++;
    }
    return Iterator(this,pos);
}
template<class K,class V>
typename HashTable<K,V>::Iterator HashTable<K,V>::end() {
    return Iterator(this,capacity);
}

template<class K,class V>
class HashTable<K,V>::Iterator {
    const HashTable<K,V>* table;
    int pos;
    hashtable::Node<K,V>* curr;
    Iterator(const HashTable<K,V>* table,int pos);
    friend class HashTable<K,V>;
public:
    hashtable::pair<K,V> operator*() const;
    Iterator& operator++();
    bool operator!=(const Iterator& it) const;
    Iterator(const Iterator&) = default;
    Iterator& operator=(const Iterator&) = default;
};

template<class K,class V>
HashTable<K,V>::Iterator::Iterator(const HashTable<K,V>* table1,int pos1) : table(table1), pos(pos1)
{
    if(pos < table1->capacity and pos>=0) {
	curr = table1->table[pos].next;
    } else {
	curr = nullptr;
    }
}

template<class K,class V>
hashtable::pair<K,V> HashTable<K,V>::Iterator::operator*() const {
    assert(curr != nullptr);
    
    hashtable::pair<K,V> p(curr->key,curr->value);
    return p;
}
template<class K,class V>
typename HashTable<K,V>::Iterator& HashTable<K,V>::Iterator::operator++() {
    if(curr == nullptr) {
	return *this;
    }
    if(curr->next != nullptr) {
	curr = curr->next;
    } else {
	pos++;
	curr = nullptr;
	while(pos < table->capacity) {
	    hashtable::Node<K,V>* arr = table->table;
	    if(arr[pos].next != nullptr) {
		curr = arr[pos].next;
		break;
	    }
	    pos++;
	}
    }
    return *this;
}
template<class K,class V>
bool HashTable<K,V>::Iterator::operator!=(const Iterator& iter) const {
    return table != iter.table or curr != iter.curr;
}

template<class K,class V>
void HashTable<K,V>::print() {
    // std::cout << "----------CHAIN-TABLE-BEGIN------------\n";
    for(auto it = begin(); it!=end(); ++it) {
	hashtable::pair<K,V> p = *it;
	std::cout << "key: " << p.first << " val: " << p.second << "\n";
    }
    // std::cout << "----------CHAIN-TABLE-END------------\n";
} 

#endif /* HASHTABLE_H */

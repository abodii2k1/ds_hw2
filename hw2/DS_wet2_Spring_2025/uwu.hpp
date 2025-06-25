
#pragma once
#include <iostream>
#include <memory>

using namespace std;

class Song ; 

class Genre {
    public:
    int id;
    weak_ptr<Song> root_in_songs ; 
    int songCount;
    
    Genre(int genreId) : id(genreId),root_in_songs(shared_ptr<Song>()) ,songCount(0) {}
};

// Song structure
class Song {
    public:
    int id;
    int merges;  // The genre this song was originally added to
    shared_ptr<Song> parent ; 
    shared_ptr<Genre> genre_root ; 
    Song(int songId, int when_merged) : id(songId) ,merges(when_merged),parent(nullptr) , genre_root(nullptr)  {}
};

int songHashKey(const int & e) ; 
int genreHashKey(const int& j); 
int genreHashKeyFunction ( const shared_ptr <Genre>& t ) ;
int intKey(const int& t) ;  
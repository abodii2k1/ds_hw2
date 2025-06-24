
#pragma once
#include <iostream>
#include <memory>

using namespace std;

class Genre {
    public:
    int id;
    int songCount;
    int howmanytimesmerged;
    bool hasMergedSince ; 
    
    Genre(int genreId) : id(genreId), songCount(0), howmanytimesmerged(0), hasMergedSince(false) {}
};

// Song structure
class Song {
    public:
    int id;
    int originalGenreId;  // The genre this song was originally added to
    
    Song(int songId, int genreId) : id(songId), originalGenreId(genreId)  {}
};

int songHashKey(const int & e) ; 
int genreHashKey(const int& j); 
int genreHashKeyFunction ( const shared_ptr <Genre>& t ) ;
int intKey(const int& t) ;  
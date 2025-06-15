
#pragma once
#include <iostream>
#include <memory>

using namespace std;

class Genre {
    public:
    int id;
    int songCount;
    int howmanytimesmerged;
    
    Genre(int genreId) : id(genreId), songCount(0), howmanytimesmerged(0) {}
};

// Song structure
class Song {
    public:
    int id;
    int originalGenreId;  // The genre this song was originally added to
    
    Song(int songId, int genreId) : id(songId), originalGenreId(genreId) {}
};

int songHashKey(const int & e) ; 
int genreHashKey(const int& j); 
int genreHashKeyFunction ( const shared_ptr <Genre>& t ) ; 
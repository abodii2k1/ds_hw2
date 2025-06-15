// dspotify25b2.cpp - Implementation based on the union-find with ranks algorithm
#include "dspotify25b2.h"
#include <algorithm>

DSpotify::DSpotify() : 
    songs(make_shared<HashTable<int,shared_ptr<Song>>>(songHashKey)), 
    genres(make_shared<HashTable<int,shared_ptr<Genre>>>(genreHashKey)), 
    genreUnionFind(make_shared<UnionFind<shared_ptr<Genre>>>(genreHashKeyFunction))
{}

DSpotify::~DSpotify() {
    // Destructor - shared_ptr will handle cleanup automatically
}

StatusType DSpotify::addGenre(int genreId) {
    if (genreId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    
    if (genres->contains(genreId)) {
        return StatusType::FAILURE;
    }
    
    try {
        shared_ptr<Genre> newGenre = make_shared<Genre>(genreId);
        genres->insert(genreId, newGenre);
        return StatusType::SUCCESS;
    } catch (...) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::addSong(int songId, int genreId) {
    if (songId <= 0 || genreId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    
    if (songs->contains(songId) || !genres->contains(genreId)) {
        return StatusType::FAILURE;
    }
    
    try {
        shared_ptr<Song> newSong = make_shared<Song>(songId, genreId);
        songs->insert(songId, newSong);
        
        // Add genre to union-find if not already there
        if (!genreUnionFind->elements.contains(genreId)) {
            shared_ptr<Genre> genre = genres->find(genreId);
            genreUnionFind->makeSet(genre);
        }
        
        // Increment song count for the genre's representative
        Node<shared_ptr<Genre>>* rep = genreUnionFind->find(genreId);
        if (rep && rep->value) {
            rep->value->songCount++;
        }
        
        return StatusType::SUCCESS;
    } catch (...) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3) {

    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 ||
        genreId1 == genreId2 || genreId1 == genreId3 || genreId2 == genreId3)
        return StatusType::INVALID_INPUT;
    if (!genres->contains(genreId1) || !genres->contains(genreId2) || genres->contains(genreId3))
        return StatusType::FAILURE;

}


output_t<int> DSpotify::getSongGenre(int songId) {
    
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    
}
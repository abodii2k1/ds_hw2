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

StatusType DSpotify::addGenre(int genreId) {//V
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

/*StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3) {

    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 ||
        genreId1 == genreId2 || genreId1 == genreId3 || genreId2 == genreId3)
        return StatusType::INVALID_INPUT;
    if (!genres->contains(genreId1) || !genres->contains(genreId2) || genres->contains(genreId3))
        return StatusType::FAILURE;

}*/
StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3) {//this is disaster
    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 ||
        genreId1 == genreId2 || genreId1 == genreId3 || genreId2 == genreId3)
        return StatusType::INVALID_INPUT;

    if (!genres->contains(genreId1) || !genres->contains(genreId2) || genres->contains(genreId3))
        return StatusType::FAILURE;

    try {
        // 1. Create and insert new genre node
        shared_ptr<Genre> newGenre = make_shared<Genre>(genreId3);
        genres->insert(genreId3, newGenre);
        genreUnionFind->makeSet(newGenre);

        // 2. Make sure old genres are in Union-Find
        if (!genreUnionFind->elements.contains(genreId1))
            genreUnionFind->makeSet(genres->find(genreId1));
        if (!genreUnionFind->elements.contains(genreId2))
            genreUnionFind->makeSet(genres->find(genreId2));

        // 3. Get root nodes
        auto* rep1 = genreUnionFind->find(genreId1);
        auto* rep2 = genreUnionFind->find(genreId2);
        auto* rep3 = genreUnionFind->find(genreId3);

        if (!rep1 || !rep2 || !rep3) return StatusType::FAILURE;

        // 4. Transfer song count to new genre
        rep3->value->songCount = rep1->value->songCount + rep2->value->songCount;
        rep1->value->songCount = 0;
        rep2->value->songCount = 0;

        // 5. Manually attach rep1 and rep2 under rep3
        auto attachUnder = [rep3](Node<shared_ptr<Genre>>* oldRoot) {
            if (oldRoot == rep3) return; // prevent self-cycle
            oldRoot->parent = rep3;
            oldRoot->relRank = oldRoot->baseRank + 1; // account for one merge
            oldRoot->baseRank = 0;
            rep3->size += oldRoot->size;
            oldRoot->size = 0;
        };

        attachUnder(rep1);
        attachUnder(rep2);

        return StatusType::SUCCESS;
    } catch (...) {
        return StatusType::ALLOCATION_ERROR;
    }
}


output_t<int> DSpotify::getSongGenre(int songId) {
    if(songId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if(!songs->contains(songId)) {
        return output_t<int>(StatusType::FAILURE);
    }
    try{
        shared_ptr<Song> song = songs->find(songId);
        if (!song) {
            return output_t<int>(StatusType::FAILURE);
        }
        
        // Find the representative genre of the song's original genre
        Node<shared_ptr<Genre>>* rep = genreUnionFind->find(song->originalGenreId);
        if (rep && rep->value) {
            return output_t<int>(rep->value->id);
        } else {
            return output_t<int>(StatusType::FAILURE);
        }
    } catch (...) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }

}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    if (genreId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if (!genres->contains(genreId)) {
        return output_t<int>(StatusType::FAILURE);
    }
    try{
        shared_ptr<Genre> genre = genres->find(genreId);
        if (!genre) {
            return output_t<int>(StatusType::FAILURE);
        }
        
        Node<shared_ptr<Genre>>* rep = genreUnionFind->find(genreId);
        if (rep && rep->value) {
            return output_t<int>(rep->value->songCount);
        } else {
            return output_t<int>(StatusType::FAILURE);
        }
    } catch (...) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    if (songId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    if (!songs->contains(songId)) {
        return output_t<int>(StatusType::FAILURE);
    }

    try {
        shared_ptr<Song> song = songs->find(songId);
        if (!song) {
            return output_t<int>(StatusType::FAILURE);
        }

        if (!genreUnionFind->elements.contains(song->originalGenreId)) {
            return output_t<int>(StatusType::FAILURE);
        }

        int rank = genreUnionFind->rank(song->originalGenreId);
        return output_t<int>(rank);
    } catch (...) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}
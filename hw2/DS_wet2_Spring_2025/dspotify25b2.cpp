// dspotify25b2.cpp
#include "dspotify25b2.h"

DSpotify::DSpotify()
  : songs(make_shared<HashTable<int,shared_ptr<Song>>>(songHashKey)),
    genres(make_shared<HashTable<int,shared_ptr<Genre>>>(genreHashKey)),
    uf(make_shared<UnionFind<int>>(intKey))
{}

DSpotify::~DSpotify() = default;

StatusType DSpotify::addGenre(int genreId) {
    if (genreId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    if (genres->contains(genreId)) {
        return StatusType::FAILURE;
    }
    try {
        auto g = make_shared<Genre>(genreId);
        genres->insert(genreId, g);
    } catch (bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType DSpotify::addSong(int songId, int genreId) {
    if (songId <= 0 || genreId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    if (songs->contains(songId)) {
        return StatusType::FAILURE;
    }
    auto g = genres->find(genreId);
    if (!g) {
        return StatusType::FAILURE;
    }
    try {
        auto song = make_shared<Song>(songId, 1);
        auto t1 = g->root_in_songs.lock() ; 
        if (t1 != nullptr) 
           {
            // attach under the existing root
            song->merges        -= t1->merges;
            song->parent         = t1;
            songs->insert(songId, song);
            g->songCount += 1;
            return StatusType::SUCCESS;
          }
          else {
          song->genre_root    = g;
          g->root_in_songs    = song;
          g->songCount        = 1;
          songs->insert(songId, song);
          return StatusType::SUCCESS;
      }
    } catch (bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}


StatusType DSpotify::mergeGenres(int g1, int g2, int g3) {
    // invalid if any ≤0 or any duplicates
    if (g1 <= 0 || g2 <= 0 || g3 <= 0
        || g1 == g2 || g2 == g3 || g1 == g3) {
        return StatusType::INVALID_INPUT;
    }
    // must have g1 and g2 existing, and g3 not yet existing
    if (!genres->contains(g1) || !genres->contains(g2) || genres->contains(g3)) {
        return StatusType::FAILURE;
    }
    int ok = 0;
    try {
        ok = uf->Modefied_Union(g1, g2, g3, genres);
    } catch (bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return ok ? StatusType::SUCCESS : StatusType::FAILURE;
}

output_t<int> DSpotify::getSongGenre(int songId) {
    if (songId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if (!songs->contains(songId)) {
        return output_t<int>(StatusType::FAILURE);
    }
    int genreId = uf->Modefied_find(songId, songs);
    return output_t<int>(genreId); 
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    if (genreId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    auto g = genres->find(genreId);
    if (!g) {
        return output_t<int>(StatusType::FAILURE);
    }
    return output_t<int>(g->songCount);  
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    if (songId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if (!songs->contains(songId)) {
        return output_t<int>(StatusType::FAILURE);
    }
    // path-compress and update merges counter
    uf->Modefied_find(songId, songs);
    auto s = songs->find(songId);
    auto cur = s->parent ;
    int sum = 0 ;  
    if(cur){
      sum += cur->merges ; 
      cur = cur->parent ; 
    }

    return output_t<int>(s->merges + sum );     
}

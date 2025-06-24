// dspotify25b2.cpp
#include "dspotify25b2.h"

DSpotify::DSpotify()
  : songs(make_shared<HashTable<int,shared_ptr<Song>>>(songHashKey)),
    genres(make_shared<HashTable<int,shared_ptr<Genre>>>(genreHashKey)),
    genreUnionFind(make_shared< UnionFind<shared_ptr<Genre>> >(genreHashKeyFunction)),
    songGenreUF(make_shared<UnionFind<int>>(intKey)),               // for song↔genre grouping
    initialSongRank(make_shared< HashTable<int,int> >(songHashKey))  // to snapshot merge-count
{}

DSpotify::~DSpotify() = default;

StatusType DSpotify::addGenre(int genreId) {
    if (genreId <= 0) 
        return StatusType::INVALID_INPUT;
    if (genres->contains(genreId)) 
        return StatusType::FAILURE;
    try {
        // 1) create the Genre object
        auto g = make_shared<Genre>(genreId);
        genres->insert(genreId, g);

        // 2) make a fresh UnionFind sentinel for this genre
        songGenreUF->makeSet(genreId);
        return StatusType::SUCCESS;
    } catch(...) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::addSong(int songId, int genreId) {
    if (songId <= 0 || genreId <= 0) 
        return StatusType::INVALID_INPUT;
    if (songs->contains(songId) || !genres->contains(genreId)) 
        return StatusType::FAILURE;
    try {
        // 1) record the song
        auto s = make_shared<Song>(songId, genreId);
        songs->insert(songId, s);

        // 2) ensure its genre sentinel exists
        if (!songGenreUF->elements.contains(genreId)) {
            // (shouldn’t happen if addGenre is correct, but just in case)
            songGenreUF->makeSet(genreId);
        }

        // 3) create the song’s own UF node, then hook it under its genre
        songGenreUF->makeSet(songId);
        songGenreUF->unionSets(songId, genreId);
        songs->find(songId)->originalGenreId = genreId ; 

        // 4) bump that genre’s songCount
        auto g = genres->find(genreId);
        g->songCount++;

        // 5) snapshot how many merges this song’s set has already seen
        int nowRank = songGenreUF->rank(songId);
        initialSongRank->insert(songId, nowRank);

        return StatusType::SUCCESS;
    } catch(...) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::mergeGenres(int g1, int g2, int g3) {
    // … your input checks …

    // 1) create and insert the new Genre
    auto ng = make_shared<Genre>(g3);
    genres->insert(g3, ng);

    // 2) make sure all three IDs have UF‐nodes
    songGenreUF->makeSet(g3);
    if (!songGenreUF->elements.contains(g1)) songGenreUF->makeSet(g1);
    if (!songGenreUF->elements.contains(g2)) songGenreUF->makeSet(g2);

    // 3) grab their current roots
    auto *rep1 = songGenreUF->find(g1);
    auto *rep2 = songGenreUF->find(g2);
    auto *rep3 = songGenreUF->find(g3);

    // 4) move songCounts (unchanged)
    int c1 = genres->find(g1)->songCount;
    int c2 = genres->find(g2)->songCount;
    ng->songCount = c1 + c2;
    genres->find(g1)->songCount = 0;
    genres->find(g2)->songCount = 0;

    // 5) attach rep1 and rep2 under rep3 *and* record the merge
    auto attachUnder = [&](Node<int>* oldRoot){
        if (oldRoot == rep3) return;
        // 5a) link
        oldRoot->parent   = rep3;
        // 5b) shift old base into rel and add one merge event
        oldRoot->relRank  = oldRoot->baseRank + 1;
        oldRoot->baseRank = 0;
        // 5c) sizes (optional—only matters for union-by-size, can skip)
        rep3->size       += oldRoot->size;
        oldRoot->size     = 0;
    };

    attachUnder(rep1);
    attachUnder(rep2);

    // 6) bump rep3’s baseRank so that rank(g3)==1 after one merge
    rep3->baseRank += 1;

    // 7) reset g1, g2 for future songs
    songGenreUF->makeSet(g1);
    songGenreUF->makeSet(g2);

    return StatusType::SUCCESS;
}

output_t<int> DSpotify::getSongGenre(int songId) {
    if (songId <= 0) 
        return output_t<int>(StatusType::INVALID_INPUT);
    if (!songs->contains(songId)) 
        return output_t<int>(StatusType::FAILURE);

    // 1) find the DSU node & the rank we recorded at insertion
    
    Node<int>* cur1 = songGenreUF->elements.find(songs->find(songId)->originalGenreId);
    Node<int>* cur = songGenreUF->elements.find(songId);
    int targetRank = initialSongRank->find(songId);

    if(targetRank == cur1->relRank){
         
          return output_t<int>(songs->find(songId)->originalGenreId);
    }
    // 2) walk upward, accumulating relRank, until we hit that snapshot point
    int acc = 0;
    while (cur->parent != cur) {
        int edge = cur->relRank;        // how many merges this edge represents
        if (acc + edge > targetRank) {
            // stepping one more edge would overshoot our insertion snapshot,
            // so the *current* node is exactly the root at insertion time
            break;
        }
        acc += edge;
        cur = cur->parent;
    }
    

    // 3) cur->value is the genre‐ID at the moment the song was added
    return output_t<int>(cur->value);
}


output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    if (genreId <= 0) 
        return output_t<int>(StatusType::INVALID_INPUT);
    if (!genres->contains(genreId)) 
        return output_t<int>(StatusType::FAILURE);
    return output_t<int>( genres->find(genreId)->songCount );
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    if (songId <= 0) 
        return output_t<int>(StatusType::INVALID_INPUT);
    if (!songs->contains(songId)) 
        return output_t<int>(StatusType::FAILURE);

    // how many merges has this song’s set seen *now* vs at insertion?
    int now   = songGenreUF->rank(songId);
    int then  = initialSongRank->find(songId);
    return output_t<int>( now - then );
}

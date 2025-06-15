#include "uwu.hpp"

int songHashKey(const int & e) {
    return e; 
}

int genreHashKey(const int& j){
    return j; 
}  

int  genreHashKeyFunction ( const shared_ptr <Genre>& t ) {
   return t->id ;     
} 
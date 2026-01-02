#include <iostream>
#include "custom_new_delete.h"

int main() {
    
    int *p = (int *)my_malloc(sizeof(int));
    *p = 30;
    std::cout << sizeof(BlockHeader) << ", " << sizeof(BlockHeader*) << "\n";
    std::cout << sizeof(size_t) << ", " << sizeof(bool) << "\n";
    std::cout << p << " -> "<< *p << "\n";
    my_free(p);

    p = new int(); // this internally calls custom operator new, size is known by type (compiler deduces this).
    // new int(someval) ==> allocates memory of one int, then calls the constructor and initilaizes with someval.
    *p = 31;
    std::cout << p << " -> "<< *p << "\n";
    delete p;  // this internally calls desctructor of data type and then call custom operator delete 
    
    return 0;
}

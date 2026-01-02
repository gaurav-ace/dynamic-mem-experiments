#include "custom_malloc_free.h"

void* operator new(size_t size) {
    void* p = my_malloc(size);
    if(!p)
        throw std::bad_alloc();
    return p;
}


void operator delete(void* p) noexcept {
    if(!p) return;
    my_free(p);
}

#include <iostream>
#include "custom_new_delete_ptr_array.h"
//#include "custom_new_delete.h"

int main() {
    
    int *p = (int *)my_malloc(sizeof(int));
    *p = 30;
    std::cout << sizeof(BlockHeader) << ", " << sizeof(BlockHeader*) << "\n";
    std::cout << sizeof(size_t) << ", " << sizeof(bool) << "\n";
    std::cout << p << " -> "<< *p << "\n";
    my_free(p);

    int* a = new int;
    *a = 10;

    int* b = new int;
    *b = 20;

    delete a;
    delete b;

    int* arr = new int[5];
    arr[0] = 1;
    arr[4] = 5;
    delete[] arr;
    return 0;
}

# dynamic memory allocation and deallocation


    memory layout of allocator : 
    size -> how much memory is alllocated starting from ptr + flags + size itself
    flags -> free, occupied etc
    T  *ptr -> actual pointer, address pointed by this is returned as part of 'malloc'.


what happens internally ?

-> void * malloc(size);
  1. request goes to allocator (glibc ptmalloc)
     Allocator : 
       - allocates free chunk
       - if not available, requests more memory from OS (brk, mmap)
  2. metadata about the usable memory is stored before the pointer to usable memory
  3. pointer to usable memory is returned.

 Disadvantages : 
  It is not type aware.
  There is no intialization done for this memory, there is no constructor called.
  


    size | flags | [a[0] | a[1] | a[2] | ...]
                    |
                    V
                   ptr points and returns this.

   [ allocator metadata ] | [ usable memory ]
         metadata            returned pointer



-> void * free(ptr);
    1. Marks chunk as free
       -> possibly merged with other adjacent free chunks
       -> memory is retained not quickly returned to OS.

That is why "use-after-free" is seen in code as that memory is still there.

    Does NOT:
        zero memory
        invalidate pointer
        prevent future use

   pointer becomes dangling pointer.


-> new :
 it allocates memory, calls constructor to bind it to object, initializes the memory.
 so ticks all the boxes that were not ticked in malloc.


-> delete : 
 calls the destructor
